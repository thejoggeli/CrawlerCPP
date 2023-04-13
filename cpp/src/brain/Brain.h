#pragma once

namespace Crawler {

class Robot;

class Brain {
public:

    Robot* robot;

    Brain();

    void SetRobot(Robot* robot);

    virtual void Init() = 0;

    virtual void Update() = 0;

    virtual void FixedUpdate() = 0;

    virtual void Destroy() = 0;

};

}