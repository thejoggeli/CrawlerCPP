#include "Leg.h"
#include <cmath>
#include <limits>
#include <cassert>
#include <iostream>
#include "math/Mathf.h"

using namespace std;

namespace Crawler {

Leg::Leg(){

    // create joints
    for(int i = 0; i < 4; i++){
        Joint* joint = new Joint();
        joints.push_back(joint);
    }

    // set joint limits
    joints[0]->limitMin = -PIf * 0.5f;
    joints[0]->limitMax = +PIf * 0.5f;
    joints[1]->limitMin = -PIf * 0.5f;
    joints[1]->limitMax = +PIf * 0.5f;
    joints[2]->limitMin = -PIf * 0.5f;
    joints[2]->limitMax = +PIf * 0.5f;
    joints[3]->limitMin = -PIf * 0.5f;
    joints[3]->limitMax = +PIf * 0.5f;
    
}

void Leg::SetHipTransform(const Eigen::Vector3f& translation, float angle){

    hipTranslation = Eigen::Affine3f::Identity();
    hipRotation = Eigen::Affine3f::Identity();
    hipTransform = Eigen::Affine3f::Identity();

    hipTranslation.translate(translation);
    hipRotation.rotate(Eigen::AngleAxisf(angle, Eigen::Vector3f::UnitZ()));
    hipTransform = hipTranslation * hipRotation;

    hipTranslationInverse = hipTranslation.inverse();
    hipRotationInverse = hipRotation.inverse();
    hipTransformInverse = hipTransform.inverse();

}

bool Leg::IKExact(const Eigen::Vector3f& Q, float phi, float angles_out[4]){

    const float L0 = joints[0]->length;
    const float L1 = joints[1]->length;
    const float L2 = joints[2]->length;
    const float L3 = joints[3]->length;

    float xy = sqrt(Q[0] * Q[0] + Q[1] * Q[1]) - L0 - L3 * sin(phi);
    float z = Q[2] + L3 * cos(phi);

    float v = (xy * xy + z * z - L1 * L1 - L2 * L2) / (2.0f * L1 * L2);

    if(v < -1.0f || v > 1.0f){
        return false;
    }
    
    float a2 = -acos(v);
    a2 = Mathf::angle_to_symmetric(a2);
    if(a2 < joints[2]->limitMin || a2 > joints[2]->limitMax){
        return false;
    }

    float a1 = atan2(z, xy) - atan2(L2 * sin(a2), L1 + L2 * cos(a2));
    a1 = Mathf::angle_to_symmetric(a1);
    if(a1 < joints[1]->limitMin || a1 > joints[1]->limitMax){
        return false;
    }

    float a3 = phi - a1 - a2 - PIf * 0.5f;
    a3 = Mathf::angle_to_symmetric(a3);
    if(a3 < joints[3]->limitMin || a3 > joints[3]->limitMax){
        return false;
    }

    float a0 = atan2(Q[1], Q[0]);
    a0 = Mathf::angle_to_symmetric(a0);
    if(a0 < joints[0]->limitMin || a0 > joints[0]->limitMax){
        return false;
    }
    
    angles_out[0] = a0;
    angles_out[1] = a1;
    angles_out[2] = a2;
    angles_out[3] = a3;
    
    return true;

}

bool Leg::IKSearch(const Eigen::Vector3f& Q, float phi_target, float angles_out[4], float angles_old[4]){

    float phi_min = -50.0f * DEG_2_RADf;
    float phi_max = +50.0f * DEG_2_RADf;
    int num_phi_vals = 256;

    bool has_result = false;
    float best_loss = numeric_limits<float>::infinity();
    float best_phi = 0.0f;
    float angles_ik[4];

    float phi_step = (phi_max - phi_min) / (float)(num_phi_vals-1);

    for(int i = 0; i < num_phi_vals; i++){
        float phi = phi_min + phi_step * (int)i;

        bool result = IKExact(Q, phi, angles_ik);

        if(result){
            float loss_val = IKLoss(phi_target, phi, angles_old, angles_ik);
            if(loss_val < best_loss){
                best_loss = loss_val;
                best_phi = phi;
                has_result = true;
            }
        }

    }

    if(has_result){
        bool result = IKExact(Q, best_phi, angles_out);
        assert(result == true);
    }

    return has_result;
    
}

float Leg::IKLoss(float phi_target, float phi_actual, float angles_old[4], float angles_new[4]){

    const float w_phi = 1.0f;
    const float w_angle = 0.5f;

    float loss = 0.0f;
    
    float dphi = phi_target - phi_actual;
    loss += ((dphi*dphi) + abs(dphi)) * w_phi;

    float da_max = 0.0f;
    for(int i = 0; i < 4; i++){
        float da = angles_old[i] - angles_new[i];
        if(da > da_max){
            da_max = da;
        }
    }

    loss += ((da_max*da_max) + abs(da_max))*w_angle;

    return loss;

}

void Leg::FKFoot(){

} 

void Leg::FKChain(){

}


}