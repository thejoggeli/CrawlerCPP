#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"

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

    bool ApplyPoseToJoints(Eigen::Vector3f footPositions[4], float footAngles[4], const Eigen::Affine3f& bodyTransform, float playtime);

};

}