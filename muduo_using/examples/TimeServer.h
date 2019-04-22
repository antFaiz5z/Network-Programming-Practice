//
// Created by faiz on 18-10-29.
//

#ifndef MUDUO_USING_TIME_SERVER_H
#define MUDUO_USING_TIME_SERVER_H

#include <muduo/net/TcpServer.h>

class TimeServer {
public:
    TimeServer(muduo::net::EventLoop *loop, const muduo::net::InetAddress &listenAddr);

    void start();

private:
    void onConnection(const muduo::net::TcpConnectionPtr &conn);

    void onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time);

    muduo::net::TcpServer server_;
};

#endif //MUDUO_USING_TIME_SERVER_H
