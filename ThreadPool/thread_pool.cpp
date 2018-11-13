//
// Created by faiz on 18-11-12.
//

#include "thread_pool.h"


inline ThreadPool2::ThreadPool2(size_t threads) : stop(false) {

    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this]{
            for(;;){
                function<void()> task;
                {
                    unique_lock<mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this]{return this->stop || !this->tasks.empty();});
                    if(this->stop && this->tasks.empty()){
                        return;
                    }
                    task = move(this->tasks.front());
                    this->tasks.pop();
                }
            }
        });
    }
}

inline ThreadPool2::~ThreadPool2() {

    {
        unique_lock<mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (thread &worker : workers){
        worker.join();
    }
}

/*template<class F, class... Args>
auto ThreadPool2::enqueue(F &&f, Args &&... args) -> future<typename result_of<F(Args...)>::type> {

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
}*/
