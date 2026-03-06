- ragel: Command not found
```bash
sudo apt install ragel
```
- fatal error: json/json.h: No such file or directory
```bash
sudo apt install libjsoncpp-dev
# 查找 jsoncpp 头文件位置, 需要将 jsoncpp/json 目录（我的 jsoncpp 在`/usr/include/jsoncpp`）
# 放在 /usr/include 目录下（这里通过软链接实现）
find / -name "jsoncpp" 2>/dev/null  
sudo ln -s /usr/include/jsoncpp/json/ /usr/include/json
```

- error: field ‘m_name’ has incomplete type ‘std::string’ </root/codes/zzt_test/proj/sylar/sylar/db/fox_thread.h:88:17>

    - 添加 `#include <string>`
    - 类似的报错还有：

        - /root/codes/zzt_test/proj/sylar/sylar/env.h:39:17
        - /root/codes/zzt_test/proj/sylar/./sylar/thread.h:80:17:

- MySQL的C++驱动报错：‘my_bool’ was not declared in this scope; did you mean ‘bool’?
    - 将类 my_bool 改为 bool。改动有两处位置，分别是：
    
        - /root/codes/zzt_test/proj/sylar/sylar/db/mysql.h:120:        my_bool is_null;
        - /root/codes/zzt_test/proj/sylar/sylar/db/mysql.h:121:        my_bool error;

- fatal error: hiredis-vip/hiredis.h: No such file or directory
```makedown
> https://zhuanlan.zhihu.com/p/599816240

一、redis的安装
（1）下载redis版本：

git clone https://gitee.com/mirrors/redis.git -b 6.2
（2）编译和安装：

cd redis
make
make test
sudo make install
# 默认安装在 /usr/local/bin
# redis-server 是服务端程序
# redis-cli 是客户端程序
（3）准备一个redis配置文件：

mkdir redis-data
# 把redis文件夹下 redis.conf 拷贝到 redis-data
# 修改 redis.conf
# requirepass 修改密码 redis，默认是没有密码
# daemonize yes
（4）启动redis-server服务：

cd redis-data
redis-server redis.conf
（5）通过 redis-cli 访问 redis-server：

# 没有密码,使用这个
redis-cli

# 如果设置了密码，使用这个，-h是主机地址
redis-cli -h 127.0.0.1 -a redis

二、hiredis-vip 库的安装
(1) 下载源码
git clone https://gitee.com/vipshop/hiredis-vip.git

(2) 编译和安装
cd hiredis-vip
make
make install
```

- /root/codes/zzt_test/proj/sylar/./sylar/zk_client.h:14:10: fatal error: zookeeper/zookeeper.h: No such file or directory
```
sudo apt install libzookeeper-mt-dev
```

- /root/codes/zzt_test/proj/sylar/./sylar/db/sqlite3.h:12:10: fatal error: sqlite3.h: No such file or directory
```
sudo apt install -y libsqlite3-dev
```

- /root/codes/zzt_test/proj/sylar/sylar/ds/roaring.c:2981:31: error: pointer ‘oldarray’ may be used after ‘realloc’ [-Werror=use-after-free]
```txt
在 CMakeList.txt 添加编译标志 -Wno-use-after-free
```

- /root/codes/zzt_test/proj/sylar/sylar/env.cc:18:13: error: ignoring return value of ‘ssize_t readlink(const char*, char*, size_t)’ declared with attribute ‘warn_unused_result’ [-Werror=unused-result]
```txt
在 CMakeList.txt 添加编译标志 -Wno-unused-result
```

- /root/codes/zzt_test/proj/sylar/sylar/util/crypto_util.cc:65:20: error: aggregate ‘EVP_CIPHER_CTX ctx’ has incomplete type and cannot be defined
   65 |     EVP_CIPHER_CTX ctx;
```txt
之所以出现这个问题，是因为系统从ubuntu16.04升级到ubuntu18.04之后，系统的openssl版本也跟着升级了。在新版的openssl中，不支持EVP_CIPHER_CTX ctx这样的扩展，但是代码中的写法未同步升级导致。

将 EVP_CIPHER_CTX ctx 改为新写法 EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new()
同时将 &ctx 改为 ctx
```

- /root/codes/zzt_test/proj/sylar/sylar/util/json_util.h:23:12: error: ‘uint32_t’ does not name a type
   23 |     static uint32_t GetUint32(const Json::Value& json

    - #include <cstdint>

- /root/codes/zzt_test/proj/sylar/sylar/util/hash_util.cc:265:5: error: ‘SHA_Init’ was not declared in this scope; did you mean ‘SHA1_Init’?
  265 |     SHA_Init(&ctx);
      |     ^~~~~~~~
      |     SHA1_Init
    
    - OpenSSL 3.0 中废弃了一些旧的 SHA 函数，现已没有SHA_INIT等函数。观察到使用SHA_INIT的`std::string sha0sum`函数并没有被调用过，遂注释该函数。

- 报错信息如下：
```zsh
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc:21:44: error: ‘ZOO_CONTAINER’ was not declared in this scope; did you mean ‘CONTAINER’?
   21 | const int ZKClient::FlagsType::CONTAINER = ZOO_CONTAINER;
      |                                            ^~~~~~~~~~~~~
      |                                            CONTAINER
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc:28:43: error: ‘ZOO_READONLY_STATE’ was not declared in this scope
   28 | const int ZKClient::StateType::READONLY = ZOO_READONLY_STATE;
      |                                           ^~~~~~~~~~~~~~~~~~
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc:29:47: error: ‘ZOO_NOTCONNECTED_STATE’ was not declared in this scope; did you mean ‘ZOO_CONNECTED_STATE’?
   29 | const int ZKClient::StateType::NOTCONNECTED = ZOO_NOTCONNECTED_STATE;
      |                                               ^~~~~~~~~~~~~~~~~~~~~~
      |                                               ZOO_CONNECTED_STATE
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc: In member function ‘bool sylar::ZKClient::reconnect()’:
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc:52:16: error: ‘zookeeper_init2’ was not declared in this scope; did you mean ‘zookeeper_init’?
   52 |     m_handle = zookeeper_init2(m_hosts.c_str(), &ZKClient::OnWatcher, m_recvTimeout, nullptr, this, 0, m_logCb);
      |                ^~~~~~~~~~~~~~~
      |                zookeeper_init
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc: In member function ‘bool sylar::ZKClient::init(const std::string&, int, watcher_callback, log_callback)’:
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc:67:16: error: ‘zookeeper_init2’ was not declared in this scope; did you mean ‘zookeeper_init’?
   67 |     m_handle = zookeeper_init2(hosts.c_str(), &ZKClient::OnWatcher, m_recvTimeout, nullptr, this, 0, lcb);
      |                ^~~~~~~~~~~~~~~
      |                zookeeper_init
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc: In member function ‘int32_t sylar::ZKClient::setServers(const std::string&)’:
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc:72:15: error: ‘zoo_set_servers’ was not declared in this scope; did you mean ‘setServers’?
   72 |     auto rt = zoo_set_servers(m_handle, hosts.c_str());
      |               ^~~~~~~~~~~~~~~
      |               setServers
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc: In member function ‘int32_t sylar::ZKClient::getConfig(std::string&, bool, Stat*)’:
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc:103:16: error: ‘ZOO_CONFIG_NODE’ was not declared in this scope
  103 |     return get(ZOO_CONFIG_NODE, val, watch, stat);
      |                ^~~~~~~~~~~~~~~
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc: In member function ‘std::string sylar::ZKClient::getCurrentServer()’:
/root/codes/zzt_test/proj/sylar/sylar/zk_client.cc:137:15: error: ‘zoo_get_current_server’ was not declared in this scope; did you mean ‘getCurrentServer’?
  137 |     auto rt = zoo_get_current_server(m_handle);
      |               ^~~~~~~~~~~~~~~~~~~~~~
      |               getCurrentServer
```
    - 使用 sudo apt install libzookeeper-mt-dev 获取的 zookeeper 版本较旧，从源码编译
```bash
# 从源码编译
git clone https://gitee.com/apache/zookeeper.git
cd zookeeper
git checkout -b release-3.7.0

# 进入项目根目录
cd zookeeper

# Maven 依赖 Java 环境，安装 Java 先
sudo apt install -y openjdk-11-jdk
java -version

readlink -f `which java`    # 获取实际的可执行文件, 可能是 /usr/lib/jvm/java-11-openjdk-amd64/bin/java
export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64  # 根据 readlink 结果设置 JAVA_HOME 环境变量，注意是根目录，不是 bin/java
echo "export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64" >> ~/.bashrc
source ~/.bashrc

# 接着安装 Maven
sudo apt install -y maven
mvn -version

mvn clean install -Pfull-build -DskipTests

```

- inlined from ‘static sylar::Uri::ptr sylar::Uri::Create(const std::string&)’ at /root/codes/zzt_test/proj/sylar/sylar/uri.rl:80:27: /usr/include/c++/13/bits/char_traits.h:445:56: error: argument 2 null where non-null expected [-Werror=nonnull]
  445 |         return static_cast<char_type*>(__builtin_memcpy(__s1, __s2, __n));

   - 在 cmakelist.txt 里添加 -Wno-nonnull 编译标志

- can't find lib -lmysqlclient_r

   - libmysqlclient_r.so​ 是 MySQL 数据库的官方 C 客户端库的多线程安全版本, 在较新版本的 MySQL 中（大约 5.5+）,通常只提供 libmysqlclient.so，但默认就是线程安全的，_r后缀的命名约定逐渐被弃用