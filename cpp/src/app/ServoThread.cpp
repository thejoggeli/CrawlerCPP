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
        
        // wait for next loop signal
        // LogDebug("ServoThread", "locking next loop");
        std::unique_lock<std::mutex> nextLoopLock(nextLoopMutex);
        nextLoopCv.wait(nextLoopLock, [&]{return nextLoopSignal == true;});
        nextLoopSignal = false;
        nextLoopLock.unlock();

        if(exitRequested){
            break;
        }

        // signal that serial comm started
        serialCommStartedSignal = true;
        serialCommStartedCv.notify_all();

        // LogDebug("ServoThread", "locking serial comm");
        uint64_t startTimeMicros = Time::GetSystemTimeMicros();

        // send new servo positions
        robot->MoveJointsToTargetSync(Time::fixedDeltaTime);

        // read measurements
        for(Joint* joint : robot->jointsList){
            // joint->ReadMeasuredStatus(true);
            Time::SleepMicros(1000);
        }

        uint64_t endTimeMicros = Time::GetSystemTimeMicros();
        serialCommTimeMicros = endTimeMicros - startTimeMicros;
        float serialCommTimeMillis = (float)(serialCommTimeMicros) * 1.0e-3f;

        // LogDebug("ServoThread", iLog << "serial comm took " << serialCommTimeMillis << " ms");

        // signal that serial comm finished
        serialCommCompleteSignal = true;
        serialCommCompleteCv.notify_all();

        loopCounter += 1;

    }

    LogInfo("ServoThread", "finished");
    finished = true;
    finishedCv.notify_all();

}

void ServoThread::RequestExit(){
    if(exitRequested){
        return;
    }
    LogInfo("ServoThread", "RequestExit()");
    exitRequested = true;
}


}