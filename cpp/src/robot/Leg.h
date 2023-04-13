#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"
#include "Joint.h"
#include <vector>

namespace Crawler {

class Leg {

public:

    std::vector<Joint*> joints;

    // transform from robot center to hip
    Eigen::Affine3f hipTranslation = Eigen::Affine3f::Identity();
    Eigen::Affine3f hipRotation = Eigen::Affine3f::Identity();
    Eigen::Affine3f hipTransform = Eigen::Affine3f::Identity();

    // transform from hip to robot center
    Eigen::Affine3f hipTranslationInverse = Eigen::Affine3f::Identity();
    Eigen::Affine3f hipRotationInverse = Eigen::Affine3f::Identity();
    Eigen::Affine3f hipTransformInverse = Eigen::Affine3f::Identity();

    Leg();

    void SetHipTransform(const Eigen::Vector3f& translation, float angle);

    bool IKExact(const Eigen::Vector3f& Q, float phi, float angles_out[4]);
    bool IKSearch(const Eigen::Vector3f& Q, float phi_target, float angles_out[4], float angles_old[4]);
    float IKLoss(float phi_target, float phi_actual, float angles_old[4], float angles_new[4]);

    void FKFoot(); 
    void FKChain();

};

}