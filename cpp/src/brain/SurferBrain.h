
#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"
#include "Brain.h"

namespace Crawler {

class SurferBrain : public Brain {

private:

    float anglesOld[4][4];
    Eigen::Vector3f stancePosFeetRelative[4];
    float stancePhi;


public:

    Eigen::Affine3f pivotTransform = Eigen::Affine3f::Identity();
    Eigen::Affine3f pivotTransformInverse = Eigen::Affine3f::Identity();

    SurferBrain();

    virtual void Init() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void Destroy() override;

    void SetStance(float xy, float z, float hipAngle, float phi);

    void SetPivot(const Eigen::Vector3f& pivot);

};


}