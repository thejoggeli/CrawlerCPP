
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
    float stanceXY;
    float stanceZ;
    float stanceHipAngle;

public:

    bool startupOnInit = true;

    int surfMode = 0;
    int stanceMode = 0;

    Eigen::Affine3f pivotTransform = Eigen::Affine3f::Identity();
    Eigen::Affine3f pivotTransformInverse = Eigen::Affine3f::Identity();
    
    float bodyTranslationSpeed = 0.2f;
    float bodyTranslationTime;
    float bodyTranslationDuration;
    Eigen::Vector3f bodyTranslationStart;
    Eigen::Vector3f bodyTranslationTarget;
    Eigen::Vector3f bodyTranslationDelta;
    Eigen::Vector3f bodyTranslation;
    Eigen::Vector3f bodyTranslationOld;

    float bodyRotationSpeed = 60.0f * DEG_2_RADf;
    float bodyRotationTime;
    float bodyRotationDuration;
    Eigen::Vector3f bodyRotationStart;
    Eigen::Vector3f bodyRotationTarget;
    Eigen::Vector3f bodyRotationDelta;
    Eigen::Vector3f bodyRotation;
    Eigen::Vector3f bodyRotationOld;

    Eigen::Affine3f bodyTransform = Eigen::Affine3f::Identity();

    SurferBrain();

    virtual void Init() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void Destroy() override;

    void SetStance(float xy, float z, float hipAngle, float phi);

    void SetPivot(const Eigen::Vector3f& pivot);

    void SetBodyTranslationTarget(float x, float y, float z);
    void SetBodyTranslationTarget(const Eigen::Vector3f& translation);

    void SetBodyRotationTarget(float x, float y, float z);
    void SetBodyRotationTarget(const Eigen::Vector3f& rotation);
    void SetbodyRotationTarget(const Eigen::Matrix3f& rotation);

    void SetBodyTranslationSpeed(float speed);
    void SetbodyRotationSpeed(float speed);

    void AutoSurf();

    // move current translation and rotation towards the target
    void SurfTowardsTarget();

    // compute the new surf transform given the current translation and rotation
    void UpdateBodyTransform();


};


}