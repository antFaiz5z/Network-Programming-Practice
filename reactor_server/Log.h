//
// Created by Faiz on 2019/4/1 0001.
//
// 使用一个队列，需要写日志时，将日志加入队列中，另外一个专门的日志线程来写日志
// 以上代码只是个简化版的实现，使用std::list来作为队列，使用条件变量来作为新日志到来的触发条件。
// 当然，由于使用了两个固定长度的数组，大小是256和512，如果日志数据太长，会导致数组溢出，
// 这个可以根据实际需求增大缓冲区或者改用动态长度的string类型。使用这两个文件只要包含Log.h，
// 然后使用如下一行代码启动日志线程就可以了：
// ServerLog::GetInstance().Start();
// 生成日志，使用头文件里面定义的三个宏LogInfo、LogWarning、LogError，当然你也可以扩展自己的日志级别。
//

#ifndef REACTORSERVER_LOG_H
#define REACTORSERVER_LOG_H

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>

//struct FILE;

#define LogInfo(...) ServerLog::GetInstance().AddToQueue("INFO", __FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
#define LogWarning(...) ServerLog::GetInstance().AddToQueue("WARNING", __FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)
#define LogError(...) ServerLog::GetInstance().AddToQueue("ERROR", __FILE__, __LINE__, __PRETTY_FUNCTION__, __VA_ARGS__)

class ServerLog {

public:

    ServerLog(const ServerLog &rhs) = delete;

    ServerLog &operator=(ServerLog &rhs) = delete;

    static ServerLog &GetInstance();

    void SetFileName(const char *filename);

    bool Start();

    void Stop();

    void AddToQueue(const char *pszLevel,
                    const char *pszFile,
                    int lineNo,
                    const char *pszFuncSig,
                    char *pszFmt, ...);

private:
    ServerLog() = default;

    void threadfunc();

private:
    std::string filename_;
    FILE *fp_{};
    std::shared_ptr<std::thread> spthread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    //有新的日志到来的标识
    bool exit_{false};
    std::list<std::string> queue_;
};


#endif //REACTORSERVER_LOG_H
