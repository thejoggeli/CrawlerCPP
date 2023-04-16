#include "Joint.h"
#include "parts/XYZServo.h"
#include "math/Mathf.h"
#include "core/Log.h"
#include "Leg.h"

namespace Crawler {

std::string JointTypeToString(JointType jointType){
    switch(jointType){
        case JointType::H0: return "H0";
        case JointType::K1: return "K1";
        case JointType::K2: return "K2";
        case JointType::K3: return "K3";
    }
    return "JointTypeStringMissing";
}

Joint::Joint(Leg* leg, JointType jointType){
    this->leg = leg;
    this->type = jointType;
    this->name = JointTypeToString(jointType);
    this->nameWithLeg = leg->name + ":" + this->name;
}

void Joint::SetServo(XYZServo* servo){
    this->servo = servo;
    char nameBuffer[20];
    sprintf(nameBuffer, "XYZServo:%03d", servo->getId());
    this->servoName = nameBuffer; 
    this->debugName = this->nameWithLeg + "/" + this->servoName;
}

bool Joint::PingServo(){
    uint16_t xyz = servo->readPosition();
    if(servo->getLastError()){
        LogError("Joint", iLog << debugName << " PingServo() failed");
        lastPingServoResult = false;
        return false;
    }
    lastPingServoResult = true;
    return true;
}

void Joint::SetServoAngleScale( float angleScale){
    this->servoAngleScale = angleScale;
}

void Joint::SetTargetAngle(float angle){
    lastTargetAngle = currentTargetAngle;
    currentTargetAngle = angle;
}

void Joint::MoveServoToTargetAngle(float seconds){
    if(seconds > 2.5f){
        LogError("Joint", iLog << debugName << " MoveServoToTargetAngle seconds=" << seconds << " is too large");
        seconds = 2.5f;
    }
    uint16_t angle = AngleToXYZ(currentTargetAngle);
    uint8_t playtime = (uint8_t)(seconds*100.0f);
    servo->setPosition(angle, playtime);
}

bool Joint::UpdateMeasuredAngle(){
    uint16_t xyz = servo->readPosition();
    if(servo->getLastError()){
        LogError("Joint", iLog << debugName << " readPosition() failed");
        return false;
    }
    measuredAngle = XYZToAngle(xyz);
    return true;
}

bool Joint::UpdateMeasuredCurrent(){
    uint16_t iBus = servo->readCurrent();
    if(servo->getLastError()){
        LogError("Joint", iLog << debugName << " readCurrent() failed");
        return false;
    }
    measuredCurrent = (float)iBus * (1000.0f / 200.0f);
    return true;
}

bool Joint::UpdateMeasuredVoltage(){
    uint8_t voltage = servo->readVoltage();
    if(servo->getLastError()){
        LogError("Joint", iLog << debugName << " readVoltage() failed");
        return false;
    }
    measuredVoltage = (float)voltage * (1.0f / 200.0f);
    return true;
}

bool Joint::UpdateMeasuredTemperature(){
    uint8_t temperature = servo->readTemperature();
    if(servo->getLastError()){
        LogError("Joint", iLog << debugName << " readTemperature() failed");
        return false;
    }
    measuredTemperature = (float)temperature;
    return true;
}

bool Joint::UpdateStatusError(){
    uint8_t statusError = servo->readStatusError();
    if(servo->getLastError()){
        LogError("Joint", iLog << debugName << " readStatusError() failed");
        return false;
    }
    this->statusError = statusError;
    return true;
}

bool Joint::UpdateStatusDetail(){
    uint8_t statusDetail = servo->readStatusDetail();
    if(servo->getLastError()){
        LogError("Joint", iLog << debugName << " readStatusDetail() failed");
        return false;
    }
    this->statusDetail = statusDetail;
    return true;
}

float Joint::XYZToAngle(uint16_t xyz){
    return (xyz - 511.0f) / servoAngleScale * PIf * (1.0f/511.0f);
}

uint16_t Joint::AngleToXYZ(float angle){
    return (uint16_t)(servoAngleScale * angle / PIf * 511.0f + 511.0f);
}

void Joint::SetServoLedPolicyUser(){
    servo->setLedPolicy(XYZServoLedPolicy::UserAll);
}

void Joint::SetServoLedPolicySystem(){
    servo->setLedPolicy(XYZServoLedPolicy::SystemAlarmAll);
}

void Joint::SetServoLedColor(int r, int g, int b, int w){

    // compress color into a single int 
    unsigned int servoLedColor = r | (g<<1) | (b<<2) | (w<<3);

    // only send new led color if servo led is not already at color
    if(servoLedColor != this->servoLedColor){
        servo->setLedColor(r, g, b, w); // set servo led to new color
        this->servoLedColor = servoLedColor; // store compressed color
    }

}

void Joint::TorqueOff(){
    servo->torqueOff();
}


}