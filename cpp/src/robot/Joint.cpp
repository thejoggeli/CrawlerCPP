#include "Joint.h"
#include "parts/XYZServo.h"
#include "math/Mathf.h"

namespace Crawler {

Joint::Joint(){

}

void Joint::SetServo(XYZServo* servo){
    this->servo = servo;
}

void Joint::SetServoAngleScale( float angleScale){
    this->servoAngleScale = angleScale;
}

void Joint::ReadCurrentAngle(){
    // TODO
    // uint16_t xyzAngle = servo->
}

void Joint::SetTargetAngle(float angle){
    lastTargetAngle = currentTargetAngle;
    currentTargetAngle = angle;
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

void Joint::SetServoLedColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w){
    servo->setLedColor(r, g, b, w);
}


}