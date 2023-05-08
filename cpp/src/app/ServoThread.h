#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "threading/Signal.h"
#include <vector>

namespace Crawler {

class Robot;
class Joint;

class ServoThread {


private:

    Robot* robot;
    std::thread thread; 
    std::atomic<bool> exitRequested = false;
    int nextJoint = 0;
    int measureState = 0;
    int nextLeg = 0;

    std::vector<Joint*> ledPolicyVector;
    std::vector<Joint*> ledColorVector;
    std::vector<Joint*> torqueVector;

    void Run();

public: 

    std::atomic<uint64_t> loopCounter = 0;
    std::atomic<uint64_t> serialCommTimeMicros = 0;

    Signal nextLoopSignal;
    Signal serialCommStartSignal;
    Signal serialCommFinishSignal;
    Signal threadFinishSignal;

    ServoThread();

    void Init(Robot* robot);
    void Start();
    void ApplyBuffers();
    void RequestExit();

};

}
