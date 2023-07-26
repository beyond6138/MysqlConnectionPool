// #include "Connection.h"
#include "ConnectionPool.h"
#include <bits/types/clock_t.h>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <memory>

using namespace std;

int main() {
  /*
  多线程测试，采用4个线程

  不使用 连接池  1000 个用户  use time 16779520ms
  要使用 连接池  1000 个用户  use time   598272ms  32倍差距

  不使用 连接池  10000 个用户  use time 183337774ms
  要使用 连接池  10000 个用户  use time    889146ms  183倍差距
  */
  clock_t begin = clock();

  /*
  // 不使用 连接池
  thread t1([]() {
    for (int i = 0; i < 2500; i++) {
      Connection conn;
      char sql[1024] = {0};
      sprintf(sql, "insert into user (name, age, sex) values ('%s','%d','%s')",
              "three", 23, "male");
      conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
      conn.update(sql);
    }
  });

  thread t2([]() {
    for (int i = 0; i < 2500; i++) {
      Connection conn;
      char sql[1024] = {0};
      sprintf(sql, "insert into user (name, age, sex) values ('%s','%d','%s')",
              "three", 23, "male");
      conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
      conn.update(sql);
    }
  });

  thread t3([]() {
    for (int i = 0; i < 2500; i++) {
      Connection conn;
      char sql[1024] = {0};
      sprintf(sql, "insert into user (name, age, sex) values ('%s','%d','%s')",
              "three", 23, "male");
      conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
      conn.update(sql);
    }
  });

  thread t4([]() {
    for (int i = 0; i < 2500; i++) {
      Connection conn;
      char sql[1024] = {0};
      sprintf(sql, "insert into user (name, age, sex) values ('%s','%d','%s')",
              "three", 23, "male");
      conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
      conn.update(sql);
    }
  });
  */

  // 使用 连接池
  thread t1([]() {
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    for (int i = 0; i < 2500; i++) {
      shared_ptr<Connection> sp = cp->getConnection();
      char sql[1024] = {0};
      sprintf(sql, "insert into user (name, age, sex) values('%s','%d','%s')",
              "three", 23, "male");
      sp->update(sql);
    }
  });

  thread t2([]() {
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    for (int i = 0; i < 2500; i++) {
      shared_ptr<Connection> sp = cp->getConnection();
      char sql[1024] = {0};
      sprintf(sql, "insert into user (name, age, sex) values('%s','%d','%s')",
              "three", 23, "male");
      sp->update(sql);
    }
  });

  thread t3([]() {
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    for (int i = 0; i < 2500; i++) {
      shared_ptr<Connection> sp = cp->getConnection();
      char sql[1024] = {0};
      sprintf(sql, "insert into user (name, age, sex) values('%s','%d','%s')",
              "three", 23, "male");
      sp->update(sql);
    }
  });

  thread t4([]() {
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    for (int i = 0; i < 2500; i++) {
      shared_ptr<Connection> sp = cp->getConnection();
      char sql[1024] = {0};
      sprintf(sql, "insert into user (name, age, sex) values('%s','%d','%s')",
              "three", 23, "male");
      sp->update(sql);
    }
  });

  t1.join();
  t2.join();
  t3.join();
  t4.join();
  clock_t end = clock();
  cout << " use time " << (end - begin) << "ms" << endl;

  return 0;
}