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

enum class ServoState {
    Error, 
    Ready, // default state on startup
    OK, // state will be set after successful pinging
    Rebooting, 
    TorqueOff,
};

static std::string JointTypeToString(JointType jointType);

class Joint {

private: 

    uint16_t servoCalibValues[3] = {256, 512, 768}; 
    float servoCalibAngles[3] = {0.0f, 0.0f, 0.0f};

    float servoCalibFactorA = 0.0f;
    float servoCalibFactorB = 0.0f;
    float servoCalibFactorC = 0.0f;

    bool servoCalibLinear = false;

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

    uint16_t lastTargetXYZ = 0;
    uint16_t currentTargetXYZ = 0;

    // the BufferedValue class is used to run the serial communication in a parallel thread
    // their value attribute must not be changed while the main loop is running

    // remember current servo led color 
    // bit 3 = white
    // bit 2 = blue
    // bit 1 = green
    // bit 0 = red 
    BufferedValue<unsigned int> servoLedColor;
    BufferedValue<unsigned int> servoLedPolicy;
    BufferedValue<bool> torque;
    BufferedValue<float> measuredXYZ; // radians
    BufferedValue<float> measuredAngle; // radians
    BufferedValue<float> measuredCurrent; // mA
    BufferedValue<float> measuredTemperature; // degC
    BufferedValue<float> measuredVoltage; // V
    BufferedValue<float> measuredPwm;
    BufferedValue<uint8_t> statusDetail;
    BufferedValue<uint8_t> statusError;
    
    XYZServo* servo = nullptr;
    bool lastPingServoResult = false;

    ServoState servoState = ServoState::Ready;
    bool canCommunicate = false;

    Joint(Leg* leg, JointType jointType);

    void SetCalibrationAnglesLinear(float low, float high);
    void SetCalibrationAnglesQuadratic(float low, float mid, float high);
    void GetCalibrationAngles(float values[3]);
    float GetCalibrationAngle(unsigned int number);
    void SetCalibrationValues(unsigned int values[3]);
    void GetCalibrationValues(unsigned int values[3]);
    void SetCalibrationValue(unsigned int value, unsigned int number);
    unsigned int GetCalibrationValue(unsigned int number);
    void UpdateCalibrationFactors();

    void SetServoState(ServoState state);
    void SetServo(XYZServo* servo);

    void RebootServo();
    bool PingServo();

    void SetTargetAngle(float angle);
    void MoveServoToTargetAngle(float seconds);

    bool ReadMeasuredStatus(bool buffer = false, int retries = 2); // reads: angle, pwm, current, statusError, statusDetail 
    bool ReadMeasuredAngle(bool buffer = false, int retries = 2);
    bool ReadMeasuredCurrent(bool buffer = false, int retries = 2);
    bool ReadMeasuredTemperature(bool buffer = false, int retries = 2);
    bool ReadMeasuredVoltage(bool buffer = false, int retries = 2);
    bool ReadStatusError(bool buffer = false, int retries = 2);
    bool ReadStatusDetail(bool buffer = false, int retries = 2);

    void SetServoLedPolicyUser(bool buffer = false);
    void SetServoLedPolicySystem(bool buffer = false);
    void SetServoLedColor(int r, int g, int b, int w, bool buffer = false);

    void UpdateServoLedPolicy();
    void UpdateServoLedColor();

    uint16_t AngleToXYZ(float angle);
    float XYZToAngle(uint16_t xyz);

    void TorqueOff(bool buffer = false);
    void TorqueOn(bool buffer = false);
    void UpdateTorque();


};    

}