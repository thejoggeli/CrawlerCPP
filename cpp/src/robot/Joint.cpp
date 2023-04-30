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

void Joint::SetServoState(ServoState state){
    switch(state){
        case ServoState::OK:
        case ServoState::TorqueOff:
            canCommunicate = true;
            break;
        default:
            canCommunicate = false;
            break;
    }
    servoState = state;
}

void Joint::SetServo(XYZServo* servo){
    this->servo = servo;
    char nameBuffer[20];
    sprintf(nameBuffer, "XYZServo:%03d", servo->getId());
    this->servoName = nameBuffer; 
    this->debugName = this->nameWithLeg + "/" + this->servoName;

    // override PID values
    uint16_t pid_buffer[3] = {
        0x0F00, 0x0800, 0x0000, 
    };
    servo->ramWrite(24, (uint8_t*)pid_buffer, 6);

    // override min and max voltage
    uint8_t voltage_buffer[2] = {
        (uint8_t)(10.0f*16.0f), // min voltage
        (uint8_t)(14.5f*16.0f), // max voltage
    };
    servo->ramWrite(6, voltage_buffer, 2);

}

void Joint::RebootServo(){
    SetServoState(ServoState::Rebooting);
    servo->reboot();
}

bool Joint::PingServo(){
    uint16_t xyz = servo->readPosition();
    if(servo->getLastError()){
        SetServoState(ServoState::Error);
        LogError("Joint", iLog << debugName << " PingServo() failed");
        lastPingServoResult = false;
        return false;
    }
    LogInfo("Joint", iLog << debugName << " PingServo() OK");
    lastPingServoResult = true;
    SetServoState(ServoState::OK);
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
        LogWarning("Joint", iLog << debugName << " MoveServoToTargetAngle seconds=" << seconds << " is too large");
        seconds = 2.5f;
    }
    uint16_t angle = AngleToXYZ(currentTargetAngle);
    uint8_t playtime = (uint8_t)(seconds*100.0f);
    servo->setPosition(angle, playtime);
}

bool Joint::ReadMeasuredStatus(bool buffer, int retries){
    if(!canCommunicate){
        return false;
    }
    XYZServoStatus status = servo->readStatus();
    if(servo->getLastError()){
        if(retries > 0){
            LogWarning("Joint", iLog << debugName << " ReadMeasuredStatus() failed, retries=" << retries);
            return ReadMeasuredStatus(buffer, retries-1);
        } else {
            SetServoState(ServoState::Error);
            LogError("Joint", iLog << debugName << " ReadMeasuredStatus() failed");
            return false;
        }
    }
    if(buffer){
        measuredAngle.BufferValue(XYZToAngle(status.position));
        measuredCurrent.BufferValue((float)(status.iBus) * (1000.0f / 200.0f));
        measuredPwm.BufferValue((float)(status.pwm));
        statusDetail.BufferValue(status.statusDetail);
        statusError.BufferValue(status.statusError);
    } else {
        measuredAngle.SetValue(XYZToAngle(status.position));
        measuredCurrent.SetValue((float)(status.iBus) * (1000.0f / 200.0f));
        measuredPwm.SetValue((float)(status.pwm));
        statusDetail.SetValue(status.statusDetail);
        statusError.SetValue(status.statusError);
    }
    return true;
}

bool Joint::ReadMeasuredAngle(bool buffer, int retries){
    if(!canCommunicate){
        return false;
    }
    uint16_t xyz = servo->readPosition();
    if(servo->getLastError()){
        if(retries > 0){
            LogWarning("Joint", iLog << debugName << " ReadMeasuredAngle() failed, retries=" << retries);
            return ReadMeasuredAngle(buffer, retries-1);
        } else {
            SetServoState(ServoState::Error);
            LogError("Joint", iLog << debugName << " ReadMeasuredAngle() failed");
            return false;
        }
    }
    if(buffer){
        measuredAngle.BufferValue(XYZToAngle(xyz));
    } else {
        measuredAngle.SetValue(XYZToAngle(xyz));
    }
    return true;
}

bool Joint::ReadMeasuredCurrent(bool buffer, int retries){
    if(!canCommunicate){
        return false;
    }
    uint16_t iBus = servo->readCurrent();
    if(servo->getLastError()){
        if(retries > 0){
            LogWarning("Joint", iLog << debugName << " ReadMeasuredCurrent() failed, retries=" << retries);
            return ReadMeasuredCurrent(buffer, retries-1);
        } else {
            SetServoState(ServoState::Error);
            LogError("Joint", iLog << debugName << " ReadMeasuredCurrent() failed");
            return false;
        }
    }
    if(buffer){
        measuredCurrent.BufferValue((float)iBus * (1000.0f / 200.0f));
    } else {
        measuredCurrent.SetValue((float)iBus * (1000.0f / 200.0f));
    }
    return true;
}

bool Joint::ReadMeasuredVoltage(bool buffer, int retries){
    if(!canCommunicate){
        return false;
    }
    uint8_t voltage = servo->readVoltage();
    if(servo->getLastError()){
        if(retries > 0){
            LogWarning("Joint", iLog << debugName << " ReadMeasuredVoltage() failed, retries=" << retries);
            return ReadMeasuredVoltage(buffer, retries-1);
        } else {
            SetServoState(ServoState::Error);
            LogError("Joint", iLog << debugName << " ReadMeasuredVoltage() failed");
            return false;
        }
    }
    if(buffer){
        measuredVoltage.BufferValue((float)voltage * (1.0f / 16.0f));
    } else {
        measuredVoltage.SetValue((float)voltage * (1.0f / 16.0f));
    }
    return true;
}

bool Joint::ReadMeasuredTemperature(bool buffer, int retries){
    if(!canCommunicate){
        return false;
    }
    uint8_t temperature = servo->readTemperature();
    if(servo->getLastError()){
        if(retries > 0){
            LogWarning("Joint", iLog << debugName << " ReadMeasuredTemperature() failed, retries=" << retries);
            return ReadMeasuredTemperature(buffer, retries-1);
        } else {
            SetServoState(ServoState::Error);
            LogError("Joint", iLog << debugName << " ReadMeasuredTemperature() failed");
            return false;
        }
    }
    if(buffer){
        measuredTemperature.BufferValue((float)temperature);
    } else {
        measuredTemperature.SetValue((float)temperature);
    }
    return true;
}

bool Joint::ReadStatusError(bool buffer, int retries){
    if(!canCommunicate){
        return false;
    }
    uint8_t statusError = servo->readStatusError();
    if(servo->getLastError()){
        if(retries > 0){
            LogWarning("Joint", iLog << debugName << " ReadStatusError() failed, retries=" << retries);
            return ReadStatusError(buffer, retries-1);
        } else {
            SetServoState(ServoState::Error);
            LogError("Joint", iLog << debugName << " ReadStatusError() failed");
            return false;
        }
    }
    if(buffer){
        this->statusError.BufferValue(statusError);
    } else {
        this->statusError.SetValue(statusError);
    }
    return true;
}

bool Joint::ReadStatusDetail(bool buffer, int retries){
    if(!canCommunicate){
        return false;
    }
    uint8_t statusDetail = servo->readStatusDetail();
    if(servo->getLastError()){
        if(retries > 0){
            LogWarning("Joint", iLog << debugName << " ReadStatusDetail() failed, retries=" << retries);
            return ReadStatusDetail(buffer, retries-1);
        } else {
            SetServoState(ServoState::Error);
            LogError("Joint", iLog << debugName << " ReadStatusDetail() failed");
            return false;
        }
    }
    if(buffer){
        this->statusDetail.BufferValue(statusDetail);
    } else {
        this->statusDetail.SetValue(statusDetail);
    }
    return true;
}

float Joint::XYZToAngle(uint16_t xyz){
    return (xyz - servoAngleZero) / servoAngleScale * PIf * (1.0f/servoAngleZero);
}

uint16_t Joint::AngleToXYZ(float angle){
    return (uint16_t)(servoAngleScale * angle / PIf * servoAngleZero + servoAngleZero);
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
    if(canCommunicate){
        SetServoState(ServoState::TorqueOff);
        servo->torqueOff();
    }
}


}