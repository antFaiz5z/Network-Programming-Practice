//
// Created by Faiz on 2019/3/28 0028.
//

#ifndef REACTORSERVER_REACTORSERVER_H
#define REACTORSERVER_REACTORSERVER_H

#include <list>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>

#define WORK_THREAD_NUM     5

using std::shared_ptr;
using std::thread;
using std::mutex;
using std::condition_variable;
using std::list;

class ReactorServer {

public:
    ReactorServer() = default;

    ~ReactorServer() = default;

    bool init(const char *ip, short port);

    bool uninit();

    bool close_client(int client_fd);

    static void *main_loop(void *p);

private:

    ReactorServer(const ReactorServer &rhs);

    ReactorServer &operator=(const ReactorServer &rhs);

    bool create_server_listener(const char *ip, short port);

    static void accept_thread_func(ReactorServer *server);

    static void work_thread_func(ReactorServer *server);

private:

    int m_listen_fd = 0;
    int m_epoll_fd = 0;
    bool m_stop = false;

    shared_ptr<thread> m_accept_thread;
    shared_ptr<thread> m_work_threads[WORK_THREAD_NUM];

    condition_variable m_accept_cond;
    mutex m_accept_mutex;

    condition_variable m_work_cond;
    mutex m_work_mutex;

    list<int> m_list_clients;

};


#endif //REACTORSERVER_REACTORSERVER_H
