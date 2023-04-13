#pragma once

#include <vector>
#include <cstdint>

class XYZServo;
class SerialStream;

namespace Crawler {

class Leg;
class Joint;
class Brain;

class Robot {
private:

    std::vector<uint16_t> servoGoalBuffer;
    std::vector<uint8_t> servoIds;

public:

    Brain* brain = nullptr;
    SerialStream* servoSerialStream = nullptr;
    XYZServo* masterServo = nullptr;

    std::vector<XYZServo*> jointServos;
    std::vector<Leg*> legs;
    std::vector<Joint*> jointsList;

    Robot();
    ~Robot();

    bool OpenSerialStream(const char* device);
    void CloseSerialStream();

    // void Update();
    // void FixedUpdate();

    void MoveJointsToTargetSync(float time);

    void SetBrain(Brain* brain);


    void Update();

    void FixedUpdate();

    void Startup();
    void Shutdown();

    void RebootServos(float sleepTime);

};

}