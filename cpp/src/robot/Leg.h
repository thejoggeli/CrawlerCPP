#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"
#include "Joint.h"
#include <vector>
#include "threading/BufferedValue.h"

namespace Crawler {

class Robot;
class WeightSensor;
class DistanceSensor;
class MuxI2C;

class Leg {
private:

    float phiMin = 0.0f;
    float phiMiax = 0.0f;
    std::vector<float> ikSearchPhiValues;

public:

    unsigned int id;

    DistanceSensor* distanceSensor;
    WeightSensor* weightSensor;

    BufferedValue<float> measuredDistance;
    BufferedValue<float> measuredWeight;

    struct FKJointsResult {
        // all positions are in the hip coordinate system
        // [0] H0 position is always (0,0,0)
        // [1] K1 position
        // [2] K2 position
        // [3] K3 position
        // [4] footPosition
        Eigen::Vector3f jointPositions[5];
        const Eigen::Vector3f& footPosition = jointPositions[4];
    };

    Robot* robot;
    std::string name;

    std::vector<Joint*> joints;

    // transform from robot center to hip
    Eigen::Affine3f hipTranslation = Eigen::Affine3f::Identity();
    Eigen::Affine3f hipRotation = Eigen::Affine3f::Identity();
    Eigen::Affine3f hipTransform = Eigen::Affine3f::Identity();

    // transform from hip to robot center
    Eigen::Affine3f hipTranslationInverse = Eigen::Affine3f::Identity();
    Eigen::Affine3f hipRotationInverse = Eigen::Affine3f::Identity();
    Eigen::Affine3f hipTransformInverse = Eigen::Affine3f::Identity();

    FKJointsResult fkJointsResult;

    Leg(Robot* robot, unsigned int id, const std::string& name);
    ~Leg();

    bool InitDistanceSensor(MuxI2C* mux, int channel);
    bool InitWeightSensor(MuxI2C* mux, int channel);

    void SetHipTransform(const Eigen::Vector3f& translation, float angle);

    bool IKExact(const Eigen::Vector3f& Q, float phi, float anglesOut[4]);

    void IKSearchConfig(int numPhiVals, float phiMin, float phiMax);
    bool IKSearch(const Eigen::Vector3f& Q, float phiTarget, float phiOld, float anglesOld[4], float anglesOut[4]);

    float IKLoss(float phiTarget, float phiOld, float phiNew, float anglesOld[4], float angleNew[4]);

    void FKJoints(float angles[4]);
    float FKPhi(float angles[4]);

    void GetJointsCurrentTargetAngles(float angles[4]);
    void GetJointsMeasuredAngles(float angles[4]);
    void GetJointsLastTargetAngles(float angles[4]);

    void TorqueOn(bool buffer = false);
    void TorqueOff(bool buffer = false);

    bool ReadMeasuredDistance(bool buffer = false);
    bool ReadMeasuredWeight(bool buffer = false);

};

}