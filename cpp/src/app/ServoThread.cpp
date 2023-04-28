#include "ServoThread.h"

#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include "core/Time.h"
#include "core/Log.h"

namespace Crawler {

ServoThread::ServoThread() {

}

void ServoThread::Init(Robot* robot){
    this->robot = robot;
}

void ServoThread::ApplyBuffers(){
    for(Joint* joint : robot->jointsList){
        joint->measuredAngle.ApplyBuffer();
        joint->measuredCurrent.ApplyBuffer();
        joint->measuredPwm.ApplyBuffer();
        joint->statusDetail.ApplyBuffer();
        joint->statusError.ApplyBuffer();
        joint->measuredTemperature.ApplyBuffer();
        joint->measuredVoltage.ApplyBuffer();
    }
}

void ServoThread::Start(){
    thread = std::thread(&ServoThread::Run, this);
    thread.detach();
}

void ServoThread::Run(){
    while(!exitRequested){
        
        // LogInfo("ServoThread", "waiting for next loop");
        std::unique_lock<std::mutex> nextLoopLock(nextLoopMutex);
        while(!nextLoopSignal){
            nextLoopCv.wait(nextLoopLock, [&]{return nextLoopSignal == true; });
        }
        nextLoopSignal = false;
        nextLoopLock.unlock();

        std::unique_lock<std::mutex> serialCommCompleteLock(serialCommMutex);
        // LogInfo("ServoThread", "starting serial comm");
        uint64_t startTimeMicros = Time::GetTimeMicros();

        // send new servo positions
        robot->MoveJointsToTargetSync(Time::fixedDeltaTime);

        // read measurements
        for(Joint* joint : robot->jointsList){
            joint->ReadMeasuredStatus(true);
        }

        uint64_t endTimeMicros = Time::GetTimeMicros();
        float commTimeMillis = (float)(endTimeMicros - startTimeMicros) * 1.0e-3f;

        LogInfo("ServoThread", iLog << "serial comm took " << commTimeMillis << " ms");

        serialCommCompleteSignal = true;
        serialCommCompleteCv.notify_all();
        serialCommCompleteLock.unlock();

    }
}

void ServoThread::RequestExit(){
    exitRequested = true;
}


}