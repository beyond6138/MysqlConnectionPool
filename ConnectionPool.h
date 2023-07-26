#include "Connection.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
using namespace std;

/*
实现连接池功能模块
*/

class ConnectionPool {
public:
  // 获取连接池对象实例
  static ConnectionPool *getConnectionPool();
  // 给外部提供接口，从连接池中获取一个可用的空闲连接
  shared_ptr<Connection> getConnection(); // 智能指针

private:
  // 构造函数私有化
  ConnectionPool();

  // 从配置文件中加载配置项
  bool loadConfigFile();

  // 运行在独立的线程中， 专门负责生产新连接
  void produceConnectionTask();

  // 扫描超过macIdTime时间的空闲连接，进行连接的回收
  void scannerConnectionTask();

  // 参数
  string _ip;             // mysql的ip
  unsigned short _port;   // mysql的端口 3306
  string _username;       // mysql 用户名
  string _password;       // mysql 登陆密码
  string _dbname;         // 连接数据库的名称
  int _intiSize;          // 连接池的初始连接量
  int _maxSize;           // 连接池的最大连接量
  int _maxFreeTime;       // 连接池最大空闲时间
  int _connectionTimeout; // 连接池获取连接的超时时间

  queue<Connection *> _connectionQue; // 存储mysql连接的队列
  mutex _queueMutex;         // 维护连接队列的线程安全互斥锁
  atomic_int _connectionCnt; // 记录连接所创建的Connection连接的总数量
  condition_variable cv; // 设置条件变量，用于连接生产线程和连接消费线程的通信
};