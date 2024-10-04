#include "chatserver.hpp"
#include "chatservice.hpp"
#include <signal.h>
#include <iostream>
using namespace std;

// 处理服务器 【CTRL + c】介绍后，重置user的状态信息
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

int main()
{
    signal(SIGINT, resetHandler);

    EventLoop loop;     // epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();    //epoll_wait以阻塞方式等待新用户连接，已连接用户的读写事件等

    return 0;
}