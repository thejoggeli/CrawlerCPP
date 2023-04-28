#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace Crawler {

class Robot;

class ServoThread {


private:

    Robot* robot;
    std::thread thread; 
    bool exitRequested = false;

    void Run();

public: 

    std::atomic<bool> nextLoopSignal = false;
    std::mutex nextLoopMutex;
    std::condition_variable nextLoopCv;

    std::atomic<bool> serialCommCompleteSignal = false;
    std::mutex serialCommMutex;
    std::condition_variable serialCommCompleteCv;

    ServoThread();

    void Init(Robot* robot);
    void Start();
    void ApplyBuffers();
    void RequestExit();

};

}
