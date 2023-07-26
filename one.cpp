#include <iostream>
#include <mysql/mysql.h>
/*
mysql_init()	获取或初始化MYSQL结构
mysql_real_connect()	连接到MySQL服务器。
mysql_query()	执行指定为“以Null终结的字符串”的SQL查询。
mysql_use_result()	初始化逐行的结果集检索。
mysql_field_count()	返回上次执行语句的结果集的列数。
mysql_fetch_row()	从结果集中获取下一行
mysql_num_fields()	返回结果集中的字段数
*/
class MyDB {
public:
  MyDB();
  ~MyDB();
  bool initDB(std::string host, std::string user, std::string pwd,
              std::string db_name);
  bool exeSQL(std::string sql);

private:
  MYSQL *connection; // 连接mysql句柄指针
  MYSQL_RES *result; // 指向查询结果的指针
  MYSQL_ROW row;     // 按行返回的查询信息
};

// mysql 初始化
MyDB::MyDB() {
  connection = mysql_init(nullptr); // 初始化数据库连接变量
  if (connection == nullptr) {
    std::cout << "mysql_init error!" << std::endl;
    exit(1);
  }
}

// 析构函数
MyDB::~MyDB() {
  if (connection != nullptr) {
    mysql_close(connection);
  }
}

bool MyDB::initDB(std::string host, std::string user, std::string pwd,
                  std::string db_name) {
  // 函数mysql_real_connect建立一个数据库连接
  // 成功返回MYSQL*连接句柄，失败返回NULL
  connection =
      mysql_real_connect(connection, host.c_str(), user.c_str(), pwd.c_str(),
                         db_name.c_str(), 3306, nullptr, 0);
  if (connection == nullptr) {
    std::cout << "mysql_real_connect error!" << std::endl;
    return false;
  }
  return true;
}

// 执行语句
bool MyDB::exeSQL(std::string sql) {
  // mysql_query()执行成功返回0，失败返回非0值.
  if (mysql_query(connection, sql.c_str()) != 0) {
    std::cout << "mysql_query error!" << std::endl;
    return false;
  } else {
    result = mysql_store_result(connection); // 获取结果集
    // mysql_field_count()返回connection查询的列数
    while ((row = mysql_fetch_row(result)) != nullptr) {
      // mysql_num_fields()返回结果集中的字段数
      for (int j = 0; j < mysql_num_fields(result); ++j) {
        std::cout << row[j] << " ";
      }
      std::cout << std::endl;
    }
    // 释放结果集的内存
    mysql_free_result(result);
  }
  return true;
}

int main() {
  MyDB db;
  db.initDB("localhost", "root", "123456", "drones");
  db.exeSQL("select * from drone");
  return 0;
}