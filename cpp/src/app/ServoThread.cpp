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
        joint->measuredXYZ.ApplyBuffer();
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
        nextLoopSignal.WaitAndClear();

        if(exitRequested){
            break;
        }

        // signal that serial comm started
        serialCommStartSignal.Set();

        // LogDebug("ServoThread", "locking serial comm");
        uint64_t startTimeMicros = Time::GetSystemTimeMicros();

        // send new servo positions
        robot->MoveJointsToTargetSync(Time::fixedDeltaTime);

        // read status measurements
        Leg* leg = robot->legs[nextLeg];
        for(Joint* joint : leg->joints){
            joint->ReadMeasuredStatus(true);
        }
        nextLeg += 1;
        if(nextLeg >= robot->legs.size()){
            nextLeg = 0;
        }

        // read temperature and voltage measurements
        Joint* joint = robot->jointsList[nextJoint];
        if(measureState == 0){
            joint->ReadMeasuredVoltage(true);
            measureState = 1;
        } else {
            joint->ReadMeasuredTemperature(true);
            measureState = 0;
            // measure another joint in next loop 
            nextJoint++;
            if(nextJoint >= robot->jointsList.size()){
                nextJoint = 0;
            }
        }

        // remember time
        uint64_t endTimeMicros = Time::GetSystemTimeMicros();
        serialCommTimeMicros = endTimeMicros - startTimeMicros;
        float serialCommTimeMillis = (float)(serialCommTimeMicros) * 1.0e-3f;

        // LogDebug("ServoThread", iLog << "serial comm took " << serialCommTimeMillis << " ms");

        // signal that serial comm finished
        serialCommFinishSignal.Set();

        loopCounter += 1;

    }

    LogInfo("ServoThread", "finished");
    threadFinishSignal.Set();

}

void ServoThread::RequestExit(){
    if(exitRequested){
        return;
    }
    LogInfo("ServoThread", "RequestExit()");
    exitRequested = true;
}


}