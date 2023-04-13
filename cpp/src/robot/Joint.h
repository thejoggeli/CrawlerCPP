#pragma once

#include <cstdint>

class XYZServo;

namespace Crawler {

class Joint {

public:

    float limitMin = -1.0f;
    float limitMax = +1.0f;
    float length = 1.0f;
    
    float currentAngle = 0.0f;
    float lastTargetAngle = 0.0f;
    float currentTargetAngle = 0.0f;

    float servoAngleScale = 1.0f;
    
    XYZServo* servo = nullptr;

    Joint();

    void SetServo(XYZServo* servo);

    void SetServoAngleScale(float angle);

    void ReadCurrentAngle();

    void SetTargetAngle(float angle);

    void SetServoLedPolicyUser();
    void SetServoLedPolicySystem();
    void SetServoLedColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w);

    uint16_t AngleToXYZ(float angle);

};    

}