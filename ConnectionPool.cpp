#include "ConnectionPool.h"
#include "public.h"
#include <bits/types/FILE.h>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <endian.h>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>

// 线程安全的懒汉单例函数接口
ConnectionPool *ConnectionPool::getConnectionPool() {
  static ConnectionPool pool;
  return &pool;
}

// 从配置文件中加载配置项
bool ConnectionPool::loadConfigFile() {
  FILE *pf = fopen("../Mysql/mysql.cnf", "r");
  if (pf == nullptr) {
    LOG("mysql.ini file is not exist !");
    return false;
  }
  while (!feof(pf)) { // feof 结尾返回 true
    char line[1024] = {0};
    fgets(line, 1024, pf); // 获取
    string str = line;
    int idx = str.find("=", 0); // 搜索
    if (idx == -1)              // 无效的配置项
    {
      continue;
    }
    int endidx = str.find("\n", idx);
    string key = str.substr(0, idx);
    string value = str.substr(idx + 1, endidx - idx - 1);
    // 赋值
    if (key == "ip") {
      _ip = value;
    } else if (key == "port") {
      _port = atoi(value.c_str());
    } else if (key == "username") {
      _username = value;
    } else if (key == "password") {
      _password = value;
    } else if (key == "dbname") {
      _dbname = value;
    } else if (key == "initSize") {
      _intiSize = atoi(value.c_str());
    } else if (key == "maxSize") {
      _maxSize = atoi(value.c_str());
    } else if (key == "maxFreeTime") {
      _maxFreeTime = atoi(value.c_str());
    } else if (key == "connectionTimeout") {
      _connectionTimeout = atoi(value.c_str());
    }
  }
  return true;
}

// 连接池的构造
ConnectionPool::ConnectionPool() {
  // 加载配置项
  if (!loadConfigFile()) {
    return;
  }
  // 创建初始数量的连接
  for (int i = 0; i < _intiSize; i++) {
    Connection *p = new Connection();
    p->connect(_ip, _port, _username, _password, _dbname);
    p->refreshAliveTime();  // 记录加入队列的时间
    _connectionQue.push(p); // 放入连接队列
    _connectionCnt++;
  }

  // 启动一个新的线程，作为连接的生产者
  // 给成员方法绑定当前对象
  thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
  produce.detach(); // 守护线程
  // 启动一个新的定时线程，扫描超过macIdTime时间的空闲连接，进行连接的回收
  thread scanner(std::bind(&ConnectionPool::produceConnectionTask, this));
  scanner.detach();
}

// 运行在独立的线程中， 专门负责生产新连接
void ConnectionPool::produceConnectionTask() {
  while (true) {
    unique_lock<mutex> lock(_queueMutex); // 上锁
    while (!_connectionQue.empty()) { // 队列不空，生产线程进入等待状态
      cv.wait(lock); // 线程阻塞在条件变量，直到被唤醒
    }
    // 连接数量没有达到上限，继续创建新的连接
    if (_connectionCnt < _maxSize) {
      Connection *p = new Connection();
      p->connect(_ip, _port, _username, _password, _dbname);
      p->refreshAliveTime();
      // 连接放入队列
      _connectionQue.push(p);
      _connectionCnt++;
    }
    // 通知消费者线程，可以消费连接了
    cv.notify_all();
  }
}

// 给外部提供接口，从连接池中获取一个可用的空闲连接
shared_ptr<Connection> ConnectionPool::getConnection() {
  // 首先判断线程池是否为空
  unique_lock<mutex> lock(_queueMutex);
  while (_connectionQue.empty()) { // 连接队列为空
    // 设定线程阻塞时间
    if (cv_status::timeout ==
        cv.wait_for(lock, chrono::milliseconds(_connectionTimeout))) {
      if (_connectionQue.empty()) { // 时间超时后，队列仍为空
        LOG("获取空闲连接超时，连接失败");
        return nullptr;
      }
    }
  }

  /*
  shared_prt智能指针析构时，会把connection资源直接delete掉，
  相当于直接调用connection的析构函数，close调 connectioon
  因此需要自定义shared_ptr的资源释放的方式。将connetion直接
  归还到 _connectionQue中
  */
  // 队列不为空,消费者进行消费
  shared_ptr<Connection> sp(_connectionQue.front(), [&](Connection *pcon) {
    unique_lock<mutex> lock(_queueMutex); // 确保线程安全
    pcon->refreshAliveTime();
    _connectionQue.push(pcon);
  });
  _connectionQue.pop(); // 队列出栈
  cv.notify_all();      // 消费完毕通知生产线程

  return sp;
}

// 扫描超过macIdTime时间的空闲连接，进行连接的回收
void ConnectionPool::scannerConnectionTask() {
  while (true) {
    // 通过sleep 模拟定时效果
    this_thread::sleep_for(chrono::seconds(_maxFreeTime));

    // 扫描整个队列，释放多余的连接
    unique_lock<mutex> lock(_queueMutex);
    while (_connectionCnt > _intiSize) {
      Connection *p = _connectionQue.front();
      if (p->getAliveTime() >= (_maxFreeTime * 1000)) {
        _connectionQue.pop();
        _connectionCnt--;
        delete p; // 调用 ～Connection()释放连接
      } else {
        // 队列头的时间没有超过，则表示整个队列的时间都没有超过最大空闲时间
        break;
      }
    }
  }
}