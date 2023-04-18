
#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"
#include "Brain.h"
#include "math/Mathf.h"

namespace Crawler {

class SurferBrain : public Brain {

private:

    Eigen::Vector3f footPositions[4];
    float stancePhi;
    int surfMode = 0;
    int stanceMode = 0;
    float stanceXY;
    float stanceZ;
    float stanceHipAngle;

public:

    Eigen::Affine3f pivotTransform = Eigen::Affine3f::Identity();
    Eigen::Affine3f pivotTransformInverse = Eigen::Affine3f::Identity();
    
    float bodyTranslationSpeed = 0.1f;
    float bodyTranslationTime;
    float bodyTranslationDuration;
    Eigen::Vector3f bodyTranslationStart;
    Eigen::Vector3f bodyTranslationTarget;
    Eigen::Vector3f bodyTranslationDelta;
    Eigen::Vector3f bodyTranslation;

    float bodyRotationSpeed = 30.0f * DEG_2_RADf;
    float bodyRotationTime;
    float bodyRotationDuration;
    Eigen::Vector3f bodyRotationStart;
    Eigen::Vector3f bodyRotationTarget;
    Eigen::Vector3f bodyRotationDelta;
    Eigen::Vector3f bodyRotation;

    Eigen::Affine3f bodyTransform = Eigen::Affine3f::Identity();

    SurferBrain();

    virtual void Init() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void Destroy() override;

    void SetStance(float xy, float z, float hipAngle, float phi);

    void SetPivot(const Eigen::Vector3f& pivot);

    void SetBodyTranslationTarget(float x, float y, float z);
    void SetbodyTranslationTarget(const Eigen::Vector3f& translation);

    void SetBodyRotationTarget(float x, float y, float z);
    void SetbodyRotationTarget(const Eigen::Vector3f& rotation);
    void SetbodyRotationTarget(const Eigen::Matrix3f& rotation);

    void SetbodyTranslationSpeed(float speed);
    void SetbodyRotationSpeed(float speed);

    void AutoSurf();

    // move current translation and rotation towards the target
    void SurfTowardsTarget();

    // compute the new surf transform given the current translation and rotation
    void UpdateBodyTransform();


};


}