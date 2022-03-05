

## Chart of Rpc framework
![img](https://github.com/Tiannia/intro_to_ai/blob/main/PhotoRepository/9c777f0536d2418a9e01d6499c7c50d1.png?raw=true)


## Notes
#### 1. [ZooKeeper Watcher](https://blog.csdn.net/weixin_43893397/article/details/103461472)

#### 2. 粘包问题？

在进行服务调用的时候如何区分**服务名**和**方法名**以及**参数**？
很简单，通过添加字段长度即可解决。

组成如下的格式即可：

    header_size service_name method_name args_size args

我们可以定义一个proto消息体来存储中间的三个字段，如下：
```proto
# 我们在框架中定义proto的message类型，进行数据头的序列化和反序列化
syntax = "proto3";

package mprpc;

message RpcHeader
{
    bytes service_name = 1; //服务的名字
    bytes method_name = 2;  //方法的名字
    uint32 args_size = 3;   //参数的大小长度
}
```

发送数据的时候怎么发？直接看代码：
```c++
//组织待发送的rpc请求的字符串
std::string send_rpc_str;
std::string rpc_header_str;
mprpc::RpcHeader rpcHeader;
rpcHeader.SerializeToString(&rpc_header_str);
send_rpc_str.insert(0, std::string((char*)&header_size, 4));//header_size，从开头开始，写4个字节，二进制存储head_size，就是一个整数
send_rpc_str += rpc_header_str;//rpcheader（对应proto的RpcHeader序列化后的结果）
send_rpc_str += args_str;//args
```

接收数据的时候怎么解析？
按数据头(4字节大小，表示service_name method_name args_size的长度)
反序列化得到 service_name method_name args_size
args(通过反序列化后的args_size进行读取)

来看一下代码：
```c++
//从字符流中读取前4个字节的内容
uint32_t header_size = 0;
recv_buf.copy((char*)&header_size, 4, 0);//从0下标位置拷贝4个字节的内容到header_size 

std::string rpc_header_str = recv_buf.substr(4, header_size);
//从第4个下标，前4个字节略过。读取包含了service_name method_name args_size 
//根据header_size读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
mprpc::RpcHeader rpcHeader;
std::string service_name;
std::string method_name;
uint32_t args_size;
if (rpcHeader.ParseFromString(rpc_header_str))
{
    //数据头反序列化成功
    service_name = rpcHeader.service_name();
    method_name = rpcHeader.method_name();
    args_size = rpcHeader.args_size();
}
//获取rpc方法参数的字符流数据
std::string args_str = recv_buf.substr(4 + header_size, args_size);
//header_size(4个字节) + header_str + args_str
```