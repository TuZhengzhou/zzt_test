#include <iostream>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/mysql_driver.h>

int main() {
  try {
    // 注册MySQL驱动程序
    sql::mysql::MySQL_Driver *driver = sql::mysql::get_mysql_driver_instance();
    std::cout << "driver ok" << std::endl;
    sql::Connection *con = driver->connect("localhost", "root", "mysql");
    std::cout << "connect ok" << std::endl;
    sql::Statement *stmt = con->createStatement();
    std::cout << "statement ok" << std::endl;
    //获取 mysql 数据库里面的user表里面的每一个user对应的值
    sql::ResultSet *res = stmt->executeQuery("SELECT * FROM mysql.user");
    std::cout << "executeQuery ok" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    while (res->next()) {
      //打印 user 的 User 属性的值
      std::cout << res->getString("User") << std::endl;
    }
    delete res;
    delete stmt;
    delete con;
  } catch (const sql::SQLException &e) { // 捕获 MySQL 异常，打印关键信息
    std::cerr << "MySQL 异常信息：" << std::endl;
    std::cerr << "错误代码：" << e.getErrorCode() << std::endl;
    std::cerr << "SQLState：" << e.getSQLState() << std::endl;
    std::cerr << "错误描述：" << e.what() << std::endl;
    return 1;
  } catch (const std::exception &e) { // 捕获其他通用异常
    std::cerr << "通用异常：" << e.what() << std::endl;
    return 1;
  }

  return 0;
}