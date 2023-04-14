
#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"
#include "Brain.h"
#include "math/Mathf.h"

namespace Crawler {

class SurferBrain : public Brain {

private:

    Eigen::Vector3f stancePosFeetRelative[4];
    float stancePhi;
    int surfMode = 0;
    int stanceMode = 0;
    float stanceXY;
    float stanceZ;
    float stanceHipAngle;

public:

    Eigen::Affine3f pivotTransform = Eigen::Affine3f::Identity();
    Eigen::Affine3f pivotTransformInverse = Eigen::Affine3f::Identity();
    
    float surfTranslationSpeed = 0.1f;
    float surfTranslationTime;
    float surfTranslationDuration;
    Eigen::Vector3f surfTranslationStart;
    Eigen::Vector3f surfTranslationTarget;
    Eigen::Vector3f surfTranslationDelta;
    Eigen::Vector3f surfTranslation;

    float surfRotationSpeed = 30.0f * DEG_2_RADf;
    float surfRotationTime;
    float surfRotationDuration;
    Eigen::Vector3f surfRotationStart;
    Eigen::Vector3f surfRotationTarget;
    Eigen::Vector3f surfRotationDelta;
    Eigen::Vector3f surfRotation;

    Eigen::Affine3f surfTransform = Eigen::Affine3f::Identity();
    Eigen::Affine3f surfTransformInverse = Eigen::Affine3f::Identity();

    SurferBrain();

    virtual void Init() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void Destroy() override;

    void SetStance(float xy, float z, float hipAngle, float phi);

    void SetPivot(const Eigen::Vector3f& pivot);

    void SetSurfTranslationTarget(float x, float y, float z);
    void SetSurfTranslationTarget(const Eigen::Vector3f& translation);

    void SetSurfRotationTarget(float x, float y, float z);
    void SetSurfRotationTarget(const Eigen::Vector3f& rotation);

    void SetSurfTranslationSpeed(float speed);
    void SetSurfRotationSpeed(float speed);

    void AutoSurf();

    // move current translation and rotation towards the target
    void SurfTowardsTarget();

    // compute the new surf transform given the current translation and rotation
    void UpdateSurfTransform();

    // apply the current surf transform to the joints
    bool ApplySurfToJoints();


};


}