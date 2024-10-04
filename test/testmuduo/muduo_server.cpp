#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
using namespace std;
using namespace placeholders;
using namespace muduo;
using namespace muduo::net;

/*基于muduo网络库开发服务器程序
1. 组合TcpServer对象
2. 创建EventLoop事件循环对象的指针
3. 明确TcpServer构造函数需要什么参数，输出ChatServer的构造函数
4. 在当前服务器类的构造函数中，注册处理连接的回调函数和处理读写时间的回调函数
5. 设置服务器的现场数量，muduo会自己划分listen线程和worker线程
*/
class ChatServer
{
public:
    ChatServer(EventLoop* loop,     //时间循环            
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
    void start()
    {
        _server.start();
    }

private:
    //专门处理用户连接的创建和断开  socket listen accept
    void onConnection(const TcpConnectionPtr& conn)
    {
        if(conn->connected())
        {
            cout << "state:online: ";
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << endl;
        }
        else
        {
            cout << "state:offline: ";
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << endl;
            conn->shutdown();   //close(fd)
        }
    }


    //专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr& conn, //连接
                    Buffer* buffer,            //缓冲区
                    Timestamp time)         //接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();    //得到缓冲区的数据,及得到客户端发来的信息
        cout << "recv data: " << buf << "time: " << time.toString() << endl;
        conn->send(buf);    //把接收到的返回
        // _loop->quit();
    }

    TcpServer _server;  // #1 
    EventLoop *_loop;   // #2 epoll

};


int main()
{
    EventLoop loop;     // epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();    //epoll_wait以阻塞方式等待新用户连接，已连接用户的读写事件等

    return 0;
}