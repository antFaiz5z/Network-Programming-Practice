#include <iostream>

#include "thread_pool.h"
#include "thread_pool_origin.h"

/*
*   C++11 标准库特性
    std::thread
    std::mutex, std::unique_lock
    std::condition_variable
    std::future, std::packaged_task
    std::function, std::bind
    std::shared_ptr, std::make_shared
    std::move, std::forward
    C++11 语言特性
    Lambda 表达式
    尾置返回类型
 */

int main() {

    ThreadPool pool(4);
    //ThreadPool2 pool(4);//error

    vector<future<string>> results;

    results.reserve(8);
    for (int i = 0; i < 8; ++i) {
        results.emplace_back(
                pool.enqueue([i]{
                    cout << "begin  " << i << endl;
                    this_thread::sleep_for(chrono::seconds(1));
                    cout << "end    " << i << endl;
                    return string("thread ") + to_string(i) + string(" finished.");
                })
        );
    }
    for (auto && result : results){
        cout << result.get() << ' ';
    }
    cout << endl;
    return 0;

}