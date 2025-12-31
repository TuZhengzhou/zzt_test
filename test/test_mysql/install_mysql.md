## 安装 MySQL 服务器/客户端

```
sudo apt update
sudo apt install -y mysql-server libmysqlclient-dev
sudo service mysql start
sudo mysql -u root -p   // 默认没有密码，直接回车即可登录
```

## 安装 MySQL C++ 连接器

```
git clone https://github.com/mysql/mysql-connector-cpp.git
cd mysql-connector-cpp
git checkout 8.0
// 配置
// 安装位置默认配置为 /usr/local/mysql/connector-cpp-8.0
// 如果需要修改安装位置，可以添加 -DCMAKE_INSTALL_PREFIX=/your/desired/path
// 默认情况下，构建会创建动态（共享）库。要构建静态库，请启用 BUILD_STATIC 选项, 即添加 -DBUILD_STATIC=ON
// 默认情况下，不会构建传统的 JDBC 连接器。要将 JDBC 连接器包含在构建中，请启用 WITH_JDBC 选项, 即添加 -DWITH_JDBC=ON
mkdir build && cd build
cmake .. -DWITH_JDBC=ON -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
sudo make install
sudo ldconfig
```

### 验证安装

如果使用 apt 等包管理器安装 MySQL, 需单独安装测试包
```
apt install mysql-testsuite
```

接下来，编译测试程序
```
mkdir test_mysql_cpp
cd test_mysql_cpp
cmake -DWITH_CONCPP=/usr/local/mysql/connector-c++-9.5.0 ../testapp -DWITH_JDBC=ON -DCMAKE_BUILD_TYPE=Release      // 这里的BUILD_TYPE需要和上面的一致
make -j$(nproc)
```

上述命令应该在构建位置的 run 目录中创建 devapi_test 和 xapi_test 程序。如果在配置测试程序时启用了 WITH_JDBC，则构建还会创建 jdbc_test 程序。

运行测试程序之前，请确保 MySQL 服务器实例正在运行并且启用了 X Plugin。最简单的方法是使用 MySQL 发行版中的 mysql-test-run.pl 脚本（`find / -name "mysql-test-run.pl" 2>/dev/null` 获取其位置）。对于 MySQL 8.0，默认情况下启用 X Plugin，因此请在该发行版的 mysql-test 目录中调用以下命令

```
perl mysql-test-run.pl --start-and-exit
```
该命令应该启动一个测试服务器实例，该实例启用了 X Plugin 并侦听端口 1300130209 而不是其标准端口（33060）。

现在您可以运行其中一个测试程序了。它们接受连接字符串参数，因此如果服务器如上所述启动，您可以像这样运行它们


run/devapi_test mysqlx://root@127.0.0.1:13020
run/xapi_test mysqlx://root@127.0.0.1:13020
run/jdbc_test tcp://127.0.0.1:13020 root




