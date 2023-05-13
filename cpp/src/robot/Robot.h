#pragma once

#include <vector>
#include <cstdint>

class XYZServo;
class SerialStream;
class I2CDevice;

namespace Crawler {

class Leg;
class Joint;
class Brain;

class Robot {
private:

    std::vector<uint8_t> servoIds;

public:

    Brain* brain = nullptr;
    SerialStream* servoSerialStream = nullptr;
    I2CDevice* i2cDevice = nullptr;
    XYZServo* masterServo = nullptr;

    std::vector<XYZServo*> jointServos;
    std::vector<Leg*> legs;
    std::vector<Joint*> jointsList;

    Robot();
    ~Robot();

    bool OpenSerialStream(const char* device);
    void CloseSerialStream();

    bool OpenI2CDevice(const char* device);
    void CloseI2CDevice();

    void Init();

    // void Update();
    // void FixedUpdate();

    void MoveJointsToTargetSync(float time, bool forceTorqueOn = false);

    void SetBrain(Brain* brain);

    void Update();
    void FixedUpdate();

    void Startup();
    void Shutdown();
    
    bool PingServos();
    void RebootServos(float sleepTime = 3.5f);

    void SetServosLedPolicyUser(bool buffer = false);
    void SetServosLedPolicySystem(bool buffer = false);

    void PrintServoStatus();

    void TorqueOn(bool buffer = false);
    void TorqueOff(bool buffer = false);

};

}