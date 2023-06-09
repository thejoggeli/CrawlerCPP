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
class MuxI2C;
class I2CBus;
class IMU;

class Robot {
private:

    std::vector<uint8_t> servoIds;

public:

    IMU* imu = nullptr;

    I2CBus* bus0 = nullptr;
    I2CBus* bus1 = nullptr;

    MuxI2C* mux0 = nullptr;
    MuxI2C* mux1 = nullptr;

    Brain* brain = nullptr;
    SerialStream* servoSerialStream = nullptr;
    XYZServo* masterServo = nullptr;

    std::vector<XYZServo*> jointServos;
    std::vector<Leg*> legs;
    std::vector<Joint*> jointsList;

    Robot();
    ~Robot();

    bool OpenSerialStream(const char* device);
    bool OpenI2C();

    bool Init();

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

    void ReadIMU(bool buffer = false);

};

}