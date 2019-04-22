//
// Created by Faiz on 2019/4/22 0022.
//

#include "DiscardServer.h"

#include <muduo/base/Logging.h>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

DiscardServer::DiscardServer(muduo::net::EventLoop *loop, const muduo::net::InetAddress &listenAddr)
        : server_(loop, listenAddr, "DiscardServer") {
    server_.setConnectionCallback(std::bind(&DiscardServer::onConnection, this, _1));
    server_.setMessageCallback(std::bind(&DiscardServer::onMessage, this, _1, _2, _3));
}

void DiscardServer::start() {
    server_.start();
}

void DiscardServer::onConnection(const muduo::net::TcpConnectionPtr &conn) {
    LOG_INFO << "DiscardServer - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is " << (conn->connected() ? "UP" : "DOWN");
}

void
DiscardServer::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time) {

    muduo::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " discards " << msg.size()
             << " bytes received at " << time.toString();
}






