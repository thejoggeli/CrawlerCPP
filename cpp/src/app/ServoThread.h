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
    std::atomic<bool> exitRequested = false;

    void Run();

public: 

    std::atomic<uint64_t> loopCounter = 0;
    std::atomic<uint64_t> serialCommTimeMicros = 0;

    std::atomic<bool> finished = false;
    std::mutex finishedMutex;
    std::condition_variable finishedCv;

    std::atomic<bool> nextLoopSignal = false;
    std::mutex nextLoopMutex;
    std::condition_variable nextLoopCv;

    std::atomic<bool> serialCommStartedSignal = false;
    std::mutex serialCommStartedMutex;
    std::condition_variable serialCommStartedCv;

    std::atomic<bool> serialCommCompleteSignal = false;
    std::mutex serialCommCompleteMutex;
    std::condition_variable serialCommCompleteCv;

    ServoThread();

    void Init(Robot* robot);
    void Start();
    void ApplyBuffers();
    void RequestExit();

};

}
