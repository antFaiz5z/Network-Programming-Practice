//
// Created by Faiz on 2019/4/22 0022.
//

#ifndef MUDUO_USING_DAYTIMESERVER_H
#define MUDUO_USING_DAYTIMESERVER_H

#include<muduo/net/TcpServer.h>

class DayTimeServer {

public:

    DayTimeServer(muduo::net::EventLoop *loop, const muduo::net::InetAddress &listenAddr);

    void start();

private:

    void onConnection(const muduo::net::TcpConnectionPtr &conn);
    void onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time);

    muduo::net::TcpServer server_;
};


#endif //MUDUO_USING_DAYTIMESERVER_H
