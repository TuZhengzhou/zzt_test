### 发送和接受文件

执行以下命令以测试
```
make
./server
./client
```

client 尝试连接 server，连接成功后，server 首先给 client 发送唯一标识符 uuid，后续 client 发送消息时使用该标识符作为自身标识。由于仅用于文件传输，数据头设计很简单，仅包含 uuid 和当前数据包长度。其中数据包长度段用于防止粘包。

client 阻塞式发送和接收，server 非阻塞式发送和接收

client 文件中会以 20 个客户端身份向 server 发送同一份文件，server 接收后将其保存为 20 份命名不同的文件
