//
// Created by Faiz on 2019/3/28 0028.
//

#include "ReactorServer.h"

#include <iostream>
#include <list>
#include <sstream>
#include <iomanip>   //for std::setw()/setfill()
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>  //for htonl() and htons()
#include <fcntl.h>

#define min(a, b) ((a <= b) ? (a) : (b))

using std::cout;
using std::endl;
using std::this_thread::get_id;
using std::this_thread::sleep_for;
using std::unique_lock;
using std::string;
using std::ostringstream;
using std::setw;
using std::setfill;
using std::chrono::milliseconds;

bool ReactorServer::init(const char *ip, short port) {

    if (!create_server_listener(ip, port)) {
        cout << "Unable to bind: " << ip << ":" << port << endl;
        return false;
    }
    cout << "Main thread id: " << get_id() << endl;

    m_accept_thread.reset(new thread(ReactorServer::accept_thread_func, this));
    for (auto &th : m_work_threads) {
        th.reset(new thread(ReactorServer::work_thread_func, this));
    }

    return true;
}

bool ReactorServer::uninit() {//  未调用

    m_stop = true;
    m_accept_cond.notify_one();
    m_work_cond.notify_all();

    m_accept_thread->join();
    for (auto &th : m_work_threads) {
        th->join();
    }

    ::epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_listen_fd, nullptr);
    ::shutdown(m_listen_fd, SHUT_RDWR);
    ::close(m_listen_fd);
    ::close(m_epoll_fd);

    return true;
}

bool ReactorServer::close_client(int client_fd) {

    if (-1 == ::epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, client_fd, nullptr)) {
        cout << "Close client socket failed as call epoll_ctl failed." << endl;
    }
    ::close(client_fd);
    return true;
}

bool ReactorServer::create_server_listener(const char *ip, short port) {

    m_listen_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (m_listen_fd == -1) return false;

    int on = 1;
    ::setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));//
    ::setsockopt(m_listen_fd, SOL_SOCKET, SO_REUSEPORT, (char *) &on, sizeof(on));//

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    if (-1 == ::bind(m_listen_fd, (sockaddr *) &addr, sizeof(addr))) return false;
    if (-1 == ::listen(m_listen_fd, 50)) return false;
    if (-1 == (m_epoll_fd = ::epoll_create(1))) return false;

    struct epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.events = EPOLLIN | EPOLLRDHUP;//
    ev.data.fd = m_listen_fd;
    if (-1 == ::epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_listen_fd, &ev)) return false;

    return true;
}

void *ReactorServer::main_loop(void *p) {

    cout << "Main thread id: " << get_id() << endl;

    ReactorServer *server = static_cast<ReactorServer *>(p);

    while (!server->m_stop) {
        struct epoll_event ev[1024];
        int n = ::epoll_wait(server->m_epoll_fd, ev, 1024, 10);
        if (n == 0) continue;
        else if (n < 0) {
            cout << "epoll_wait error. " << endl;
            continue;
        }
        int m = min(n, 1024);
        for (int i = 0; i < m; ++i) {
            if (ev[i].data.fd == server->m_listen_fd) {
                server->m_accept_cond.notify_one();
            } else {
                {
                    unique_lock<mutex> guard(server->m_work_mutex);
                    server->m_list_clients.push_back(ev[i].data.fd);
                }
                server->m_work_cond.notify_one();
            }
        }
    }
    cout << "Main loop exit..." << endl;
    return nullptr;
}

void ReactorServer::accept_thread_func(ReactorServer *server) {

    cout << "accept thread id: " << get_id() << endl;
    while (true) {
        int new_fd;
        struct sockaddr_in client_addr;
        socklen_t addr_len;
        {
            unique_lock<mutex> guard(server->m_accept_mutex);
            server->m_accept_cond.wait(guard);
            if (server->m_stop) break;
            new_fd = ::accept(server->m_listen_fd, (struct sockaddr *) &client_addr, &addr_len);
        }
        if (-1 == new_fd) continue;
        cout << "New client connected: " << ::inet_ntoa(client_addr.sin_addr)
             << ":" << ntohs(client_addr.sin_port) << endl;

        //将新socket设置为non-blocking
        int old_flag = ::fcntl(new_fd, F_GETFL, 0);//
        int new_flag = old_flag | O_NONBLOCK;
        if (-1 == fcntl(new_fd, F_SETFL, new_flag)) {//
            cout << "fcntl error, old_flag = " << old_flag << ", new_flag = " << new_flag << endl;
            continue;
        }

        struct epoll_event ev;
        memset(&ev, 0, sizeof(ev));
        ev.events = EPOLLIN | EPOLLRDHUP | EPOLLET;
        ev.data.fd = new_fd;
        if (-1 == ::epoll_ctl(server->m_epoll_fd, EPOLL_CTL_ADD, new_fd, &ev)) {
            cout << "epoll_ctl error, fd = " << new_fd << endl;
        }
    }
    cout << "Accept thread exit..." << endl;
}

void ReactorServer::work_thread_func(ReactorServer *server) {

    cout << "New work thread, thread id = " << get_id() << endl;

    while (true) {
        int client_fd;
        {
            unique_lock<mutex> guard(server->m_work_mutex);
            while (server->m_list_clients.empty()) {
                if (server->m_stop) {
                    cout << "Work thread exit..." << endl;
                    return;
                }
                server->m_work_cond.wait(guard);
            }
            client_fd = server->m_list_clients.front();
            server->m_list_clients.pop_front();
        }
        cout << endl;

        //拼接msg
        string client_msg;
        char buff[256];
        bool error = false;
        while (true) {
            memset(buff, 0, sizeof(buff));
            int cnt_recv = ::recv(client_fd, buff, sizeof(buff), 0);
            if (-1 == cnt_recv) {
                if (errno == EWOULDBLOCK) break;//
                else {
                    cout << "recv error, client disconnected, fd = " << client_fd << endl;
                    server->close_client(client_fd);
                    error = true;
                    break;
                }
            } else if (0 == cnt_recv) {
                cout << "Peer closed, client disconnected, fd = " << client_fd << endl;
                server->close_client(client_fd);
                error = true;
                break;
            }
            client_msg += buff;
        }
        if (error) continue;
        cout << "Client msg: " << client_msg << endl;

        //将消息加上时间标签后发回
        time_t now = time(nullptr);
        struct tm *now_str = localtime(&now);
        ostringstream ostime;
        ostime << "[" << now_str->tm_year + 1900 << "-"
               << setw(2) << setfill('0') << now_str->tm_mon + 1 << "-"
               << setw(2) << setfill('0') << now_str->tm_mday << " "
               << setw(2) << setfill('0') << now_str->tm_hour << ":"
               << setw(2) << setfill('0') << now_str->tm_min << ":"
               << setw(2) << setfill('0') << now_str->tm_sec << ":"
               << "] server reply: ";
        client_msg.insert(0, ostime.str());

        while (true) {
            int cnt_send = ::send(client_fd, client_msg.c_str(), client_msg.length(), 0);
            if (-1 == cnt_send) {
                if (errno == EWOULDBLOCK) {
                    sleep_for(milliseconds(10));
                    continue;
                } else {
                    cout << "send error, fd = " << client_fd << endl;
                    server->close_client(client_fd);
                    break;
                }
            }
            cout << "Send: " << client_msg;
            client_msg.erase(0, cnt_send);
            if (client_msg.empty()) break;
        }
    }
}
