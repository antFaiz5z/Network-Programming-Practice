//
// Created by faiz on 18-10-29.
//

#include "time_server.h"

#include <muduo/base/Logging.h>
#include <muduo/net/Endian.h>

using namespace muduo;
using namespace muduo::net;

time_server::time_server(muduo::net::EventLoop* loop,
                       const muduo::net::InetAddress& listenAddr)
        : server_(loop, listenAddr, "time_server")
{
    server_.setConnectionCallback(
            std::bind(&time_server::onConnection, this, _1));
    server_.setMessageCallback(
            std::bind(&time_server::onMessage, this, _1, _2, _3));
}

void time_server::start()
{
    server_.start();
}

void time_server::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    LOG_INFO << "TimeServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected() ? "UP" : "DOWN");
    if (conn->connected())
    {
        time_t now = ::time(NULL);
        int32_t be32 = sockets::hostToNetwork32(static_cast<int32_t>(now));
        conn->send(&be32, sizeof be32);
        conn->shutdown();
    }
}

void time_server::onMessage(const muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buf,
                           muduo::Timestamp time)
{
    string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " discards " << msg.size()
             << " bytes received at " << time.toString();
}
