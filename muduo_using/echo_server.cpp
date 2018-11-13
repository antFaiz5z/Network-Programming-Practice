//
// Created by faiz on 18-10-25.
//

#include "echo_server.h"

#include <muduo/base/Logging.h>

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

echo_server::echo_server(muduo::net::EventLoop *loop, const muduo::net::InetAddress &listenAddr)
        : server_(loop, listenAddr, "echo_server"){
    server_.setConnectionCallback(std::bind(&echo_server::onConnection, this, _1));
    server_.setMessageCallback(std::bind(&echo_server::onMessage, this, _1, _2, _3));
}

void echo_server::start() {
    server_.start();
}

void echo_server::onConnection(const muduo::net::TcpConnectionPtr &conn) {

    LOG_INFO << "echo_server - " << conn->peerAddress().toIpPort() << " -> "
             << conn->localAddress().toIpPort() << " is "
             << (conn->connected()? "UP" : "DOWN");
}

void echo_server::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time) {

    muduo::string msg(buf->retrieveAllAsString());
    LOG_INFO << conn->name() << " echo " << msg.size() << " bytes, "
             << "data received at " << time.toString();
    conn->send(msg);
}