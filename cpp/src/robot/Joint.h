#pragma once

#include <cstdint>
#include <string>
#include "threading/BufferedValue.h"

class XYZServo;

namespace Crawler {

class Leg;

enum class JointType {
    H0 = 0,
    K1 = 1,
    K2 = 2,
    K3 = 3,
};

static std::string JointTypeToString(JointType jointType);

class Joint {

private: 

    // remember current servo led color 
    // bit 3 = white
    // bit 2 = blue
    // bit 1 = green
    // bit 0 = red 
    unsigned int servoLedColor; 

public:

    Leg* leg;
    JointType type;
    
    std::string name;
    std::string nameWithLeg;
    std::string servoName;
    std::string debugName;

    float limitMin = -1.0f;
    float limitMax = +1.0f;
    float length = 1.0f;
    
    float lastTargetAngle = 0.0f;
    float currentTargetAngle = 0.0f;

    float servoAngleScale = 1.0f;

    BufferedValue<float> measuredAngle; // radians
    BufferedValue<float> measuredCurrent; // mA
    BufferedValue<float> measuredTemperature; // degC
    BufferedValue<float> measuredVoltage; // V
    BufferedValue<float> measuredPwm;
    BufferedValue<uint8_t> statusDetail;
    BufferedValue<uint8_t> statusError;
    
    XYZServo* servo = nullptr;
    bool lastPingServoResult = false;

    Joint(Leg* leg, JointType jointType);

    void SetServo(XYZServo* servo);

    bool PingServo();

    void SetServoAngleScale(float angle);

    void SetTargetAngle(float angle);
    void MoveServoToTargetAngle(float seconds);

    bool ReadMeasuredStatus(bool buffer = false); // reads: angle, pwm, current, statusError, statusDetail 
    bool ReadMeasuredAngle(bool buffer = false);
    bool ReadMeasuredCurrent(bool buffer = false);
    bool ReadMeasuredTemperature(bool buffer = false);
    bool ReadMeasuredVoltage(bool buffer = false);
    bool ReadStatusError(bool buffer = false);
    bool ReadStatusDetail(bool buffer = false);

    void SetServoLedPolicyUser();
    void SetServoLedPolicySystem();
    void SetServoLedColor(int r, int g, int b, int w);
    void SetServoLedColor(unsigned int rgbw);

    uint16_t AngleToXYZ(float angle);
    float XYZToAngle(uint16_t xyz);

    void TorqueOff();



};    

}