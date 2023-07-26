// #include "Connection.h"
#include "ConnectionPool.h"
#include <bits/types/clock_t.h>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <memory>

using namespace std;

int main() {

  clock_t begin = clock();
  ConnectionPool *cp = ConnectionPool::getConnectionPool();
  /*
  但线程测试

  不使用 连接池  1000 个用户  use time 6281527ms
  要使用 连接池  1000 个用户  use time  656133ms  10倍差距

  不使用 连接池  10000 个用户  use time 62943589ms
  要使用 连接池  10000 个用户  use time  6252210ms  10倍差距
  */

  /*
  // 不使用 连接池
  for (int i = 0; i < 1000; i++) {
    Connection conn;
    char sql[1024] = {0};
    sprintf(sql, "insert into user (name, age, sex) values ('%s','%d','%s')",
            "three", 23, "male");
    conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
    conn.update(sql);
  }
  */

  // 使用 连接池
  for (int i = 0; i < 10000; i++) {
    shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "insert into user (name, age, sex) values ('%s','%d','%s')",
            "three", 23, "male");
    sp->update(sql);
  }

  clock_t end = clock();
  cout << " use time " << (end - begin) << "ms" << endl;

  return 0;
}