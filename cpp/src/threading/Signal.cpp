#include "Signal.h"



Signal::Signal(){
    signal = false;
}

bool Signal::IsSet(){
    std::lock_guard<std::mutex> lock(mtx);
    return signal == true;
}

void Signal::Set(){
    std::lock_guard<std::mutex> lock(mtx);
    signal = true;
    cv.notify_all();
}

void Signal::Clear(){
    std::lock_guard<std::mutex> lock(mtx);
    signal = false;
}

void Signal::Wait(){
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&]{return signal == true;});
}

void Signal::WaitAndClear(){
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [&]{return signal == true;});
    signal = false;
}
