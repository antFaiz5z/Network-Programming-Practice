//
// Created by Faiz on 2019/4/23 0023.
//

#ifndef MUDUO_USING_DOWNLOAD3_H
#define MUDUO_USING_DOWNLOAD3_H

#include <stdio.h>
#include <unistd.h>

#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>

class FileServer {

public:

    void onHighWaterMark(const muduo::net::TcpConnectionPtr &conn, size_t len);

    void onConnection(const muduo::net::TcpConnectionPtr &conn);

    void onWriteComplete(const muduo::net::TcpConnectionPtr &conn);

    static void main();

private:

    const int kBufSize = 64 * 1024;
    const char *g_file = nullptr;
    typedef std::shared_ptr <FILE> FilePtr;

};


#endif //MUDUO_USING_DOWNLOAD3_H
