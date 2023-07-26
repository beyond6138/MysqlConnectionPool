/*
实现 Mysql 数据库操作
*/

#include <bits/types/clock_t.h>
#include <ctime>
#include <mysql/mysql.h>
#include <string>

using namespace std;

/*
  实现 Mysql 数据库操作
*/

class Connection {
public:
  Connection();
  ~Connection();
  // 连接数据库
  bool connect(string ip, unsigned short port, string user, string password,
               string dbname);
  // 更新操作 insert、delete、update
  bool update(string sql);
  // 查询操作 select
  MYSQL_RES *query(string sql);

  // 刷新连接的起始空闲时间
  void refreshAliveTime() { _alivetime = clock(); }
  // 返回存活的时间
  clock_t getAliveTime() const { return clock() - _alivetime; }

private:
  MYSQL *_conn;       // 表示和MySQL Server的一条连接
  clock_t _alivetime; // 记录进入空闲状态后的存活时间
};