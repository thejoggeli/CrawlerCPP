#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>

class Signal {

private:

    std::atomic<bool> signal;
    std::mutex mtx;
    std::condition_variable cv;

public:

    Signal();

    bool IsSet();
    void Set();
    void Clear();
    void Wait();
    void WaitAndClear();

};