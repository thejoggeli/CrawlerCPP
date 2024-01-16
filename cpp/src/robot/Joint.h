#pragma once

#include <cstdint>
#include <string>
#include "threading/BufferedValue.h"
#include <memory>

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
std::shared_ptr<std::string> ServoStateToString(ServoState state);

static std::string JointTypeToString(JointType jointType);

class Joint {

private: 

    uint16_t servoCalibValues[3] = {256, 512, 768}; 
    float servoCalibAngles[3] = {0.0f, 0.0f, 0.0f};

    float servoCalibFactorA0 = 0.0f;
    float servoCalibFactorA1 = 0.0f;    
    float servoCalibFactorB0 = 0.0f;
    float servoCalibFactorB1 = 0.0f;

    int numCalibPoints = 0;

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
    

    // the main thread buffers these values 
    // the buffered values are applied once at the beginning of each main loop
    BufferedValue<float> lastTargetAngle;
    BufferedValue<float> currentTargetAngle;
    BufferedValue<uint16_t> lastTargetXYZ;
    BufferedValue<uint16_t> currentTargetXYZ;
    BufferedValue<unsigned int> servoLedColor; // bits 3,2,1,0 = W,B,G,R
    BufferedValue<unsigned int> servoLedPolicy;
    BufferedValue<bool> torque;

    // the servo thread buffers these values
    // the buffered values are applied once at the beginning of each main loop
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

    void SetCalibrationAngles(float low, float high);
    void SetCalibrationAngles(float low, float mid, float high);
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

    void SetTargetAngle(float angle, bool buffer = false);
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