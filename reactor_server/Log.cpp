//
// Created by Faiz on 2019/4/1 0001.
//

#include "Log.h"

#include <time.h>
#include <stdio.h>
#include <memory>
#include <stdarg.h>
#include <functional>
//#include <iostream>

ServerLog &ServerLog::GetInstance() {
    static ServerLog logger;
    return logger;
}

void ServerLog::SetFileName(const char *filename) {
    filename_ = filename;
}

bool ServerLog::Start() {
    if (filename_.empty()) {
        time_t now = time(nullptr);
        struct tm *t = localtime(&now);
        char timestr[64] = {0};
        sprintf(timestr,
                "%04d%02d%02d%02d%02d%02d.imserver.log",
                t->tm_year + 1900,
                t->tm_mon + 1,
                t->tm_mday,
                t->tm_hour,
                t->tm_min,
                t->tm_sec);
        filename_ = timestr;
    }

    fp_ = fopen(filename_.c_str(), "wt+");
    if (fp_ == nullptr)
        return false;

    spthread_.reset(new std::thread(std::bind(&ServerLog::threadfunc, this)));

    //std::cout << "new log thread. " << std::endl;
    return true;
}

void ServerLog::Stop() {
    exit_ = true;
    cv_.notify_one();

    //等待时间线程结束
    spthread_->join();
}

void ServerLog::AddToQueue(const char *pszLevel, const char *pszFile,
                           int lineNo,
                           const char *pszFuncSig,
                           char *pszFmt, ...) {
    char msg[256] = {0};

    va_list vArgList;
    va_start(vArgList, pszFmt);
    vsnprintf(msg, 256, pszFmt, vArgList);
    va_end(vArgList);

    time_t now = time(nullptr);
    struct tm *tmstr = localtime(&now);
    char content[512] = {0};
    sprintf(content,
            "[%04d-%02d-%02d %02d:%02d:%02d][%s][0x%04x][%s:%d %s]%s\n",
            tmstr->tm_year + 1900,
            tmstr->tm_mon + 1,
            tmstr->tm_mday,
            tmstr->tm_hour,
            tmstr->tm_min,
            tmstr->tm_sec,
            pszLevel,
            std::this_thread::get_id(),
            pszFile,
            lineNo,
            pszFuncSig,
            msg);

    {
        std::lock_guard<std::mutex> guard(mutex_);
        queue_.emplace_back(content);
    }

    cv_.notify_one();
}

void ServerLog::threadfunc() {
    if (fp_ == nullptr) return;

    while (!exit_) {
        //写日志
        std::unique_lock<std::mutex> guard(mutex_);
        while (queue_.empty()) {
            if (exit_)
                return;

            cv_.wait(guard);
        }

        //写日志
        const std::string &str = queue_.front();
        //std:: cout << str << std::endl;
        fwrite((void *) str.c_str(), str.length(), 1, fp_);
        fflush(fp_);
        queue_.pop_front();
    }
}