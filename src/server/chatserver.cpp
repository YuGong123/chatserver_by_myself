#include "chatserver.hpp"
#include "chatservice.hpp"
#include <functional>
#include <iostream>
using namespace std;
using namespace placeholders;

#include "json.hpp"
using json = nlohmann::json;

ChatServer::ChatServer(EventLoop* loop,     //时间循环            
            const InetAddress& listenAddr,  //IP+Port
            const string& nameArg)  //服务器的名字
            :_server(loop, listenAddr, nameArg)
            ,_loop(loop)
{
    //给服务器注册用户连接的创建和回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    //非服务器注册用户读写时间回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    //设置服务器的现场数量  1个 listen线程， 3个IO线程
    _server.setThreadNum(4);
}


//开启时间循环
void ChatServer::start()
{
    _server.start();
}


//专门处理用户连接的创建和断开  socket listen accept
void ChatServer::onConnection(const TcpConnectionPtr& conn)
{
    // 客户端断开连接
    if(!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();   //close(fd)
    }
}


//专门处理用户的读写事件
void ChatServer::onMessage(const TcpConnectionPtr& conn, //连接
                Buffer* buffer,            //缓冲区
                Timestamp time)         //接收到数据的时间信息
{
    string buf = buffer->retrieveAllAsString();    //得到缓冲区的数据,及得到客户端发来的信息
    // 数据的反序列化
    json js = json::parse(buf);

    // 通关js["msgid"]获取 handler 处理函数
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());

    // 执行回调函数
    msgHandler(conn, js, time);
}