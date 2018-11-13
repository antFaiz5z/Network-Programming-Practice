//
// Created by faiz on 18-11-12.
//

#ifndef THREADPOOL_THREAD_POOL_H
#define THREADPOOL_THREAD_POOL_H

#include <vector>               // std::vector
#include <queue>                // std::queue
#include <memory>               // std::make_shared
#include <stdexcept>            // std::runtime_error
#include <thread>               // std::thread
#include <mutex>                // std::mutex,        std::unique_lock
#include <condition_variable>   // std::condition_variable
#include <future>               // std::future,       std::packaged_task
#include <functional>           // std::function,     std::bind
#include <utility>              // std::move,         std::forward

using namespace std;

class ThreadPool2 {

public:

    ThreadPool2(size_t threads);

    ~ThreadPool2();

    template<class F, class ...Args>
    auto enqueue(F &&f, Args &&... args)
    -> future<typename result_of<F(Args...)>::type>{

        using return_type = typename result_of<F(Args...)>::type;
        auto task = make_shared<packaged_task<return_type()>>(bind(forward<F>(f), forward<Args>(args)...));
        future<return_type > res = task->get_future();
        {
            unique_lock<mutex> lock(queue_mutex);
            if(stop){
                throw runtime_error("enqueue on stopped thread pool");
            }
            tasks.emplace([task]{(*task)();});
        }
        condition.notify_one();
        return res;
    }

private:

    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex queue_mutex;
    condition_variable condition;
    bool stop;
};


#endif //THREADPOOL_THREAD_POOL_H
