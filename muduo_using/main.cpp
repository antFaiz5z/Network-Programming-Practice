#include <muduo/base/Logging.h>
#include <muduo/base/Timestamp.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpConnection.h>

#include "examples/EchoServer.h"
#include "examples/TimeServer.h"
#include "examples/DiscardServer.h"
#include "examples/DayTimeServer.h"

using namespace muduo;
using namespace muduo::net;

void onConnection(const TcpConnectionPtr &conn) {

    LOG_INFO << "EchoServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
}

void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp time) {

    muduo::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
             << "data received at " << time.toString();
    conn->send(msg);
}

int main() {

    LOG_INFO << "pid = " << getpid();

    EventLoop loop;

    TcpServer echo_server1(&loop, InetAddress(2007), "muduo_using");
    echo_server1.setConnectionCallback(&onConnection);
    echo_server1.setMessageCallback(&onMessage);
    EchoServer echo_server2(&loop, InetAddress(2017));
    TimeServer time_server(&loop, InetAddress(2027));
    DiscardServer discard_server(&loop, InetAddress(2037));
    DayTimeServer daytime_server(&loop, InetAddress(2047));

    echo_server1.start();
    echo_server2.start();
    time_server.start();
    discard_server.start();
    daytime_server.start();

    loop.loop();

    return 0;
}