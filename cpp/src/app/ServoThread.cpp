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
    this->ledPolicyVector.reserve(16);
    this->ledColorVector.reserve(16);
    this->torqueVector.reserve(16);
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
        if(joint->servoLedPolicy.needsUpdate){
            joint->servoLedPolicy.ApplyBuffer();
            ledPolicyVector.push_back(joint);
        }
        if(joint->servoLedColor.needsUpdate){
            joint->servoLedColor.ApplyBuffer();
            ledColorVector.push_back(joint);
        }
        if(joint->torque.needsUpdate){
            joint->torque.ApplyBuffer();
            torqueVector.push_back(joint);
        }
        joint->currentTargetAngle.ApplyBuffer();
        joint->currentTargetXYZ.ApplyBuffer();
        joint->lastTargetAngle.ApplyBuffer();
        joint->lastTargetXYZ.ApplyBuffer();
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

        // stop if exit requested
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
        // Leg* leg = robot->legs[nextLeg];
        // for(Joint* joint : leg->joints){
        //     joint->ReadMeasuredStatus(true);
        // }
        // nextLeg += 1;
        // if(nextLeg >= robot->legs.size()){
        //     nextLeg = 0;
        // }

        // read temperature and voltage measurements
        Joint* joint = robot->jointsList[nextJoint];
        if(measureState == 0){
            joint->ReadMeasuredVoltage(true, 0);
            measureState = 1;
        } else {
            joint->ReadMeasuredTemperature(true, 0);
            measureState = 0;
            // measure another joint in next loop 
            nextJoint++;
            if(nextJoint >= robot->jointsList.size()){
                nextJoint = 0;
            }
        }

        // update led color policies
        for(Joint* joint : ledPolicyVector){
            joint->UpdateServoLedPolicy();
        }
        ledPolicyVector.clear();

        // update led color policies
        for(Joint* joint : ledColorVector){
            joint->UpdateServoLedColor();
        }
        ledColorVector.clear();

        // update torque
        for(Joint* joint : torqueVector){
            joint->UpdateTorque();
        }
        torqueVector.clear();

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