
#include "SurferBrain.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include <cmath>
#include "math/Mathf.h"
#include <cassert>
#include "core/Log.h"
#include "core/Time.h"

using namespace std;

namespace Crawler {


SurferBrain::SurferBrain() : Brain() {
}

void SurferBrain::Init(){
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4; j++){
            anglesOld[i][j] = 0.0f;   
        }
    }
}

void SurferBrain::Update(){

}

void SurferBrain::FixedUpdate(){

    float surf_freq = 0.2;

    float surf_dx = 0.0f;
    float surf_dy = 0.0f;
    float surf_dz = 0.0f;

    // float surf_dx = cos(PI2f * surf_freq * Time::currentTime * 1.0f) * 0.03f;
    // float surf_dy = sin(PI2f * surf_freq * Time::currentTime * 1.0f) * 0.03f;
    // float surf_dz = cos(PI2f * surf_freq * Time::currentTime * 1.0f) * 0.05f;

    // float surf_rotx = 0.0f;
    // float surf_roty = 0.0f;
    float surf_rotz = 0.0f;

    float surf_rotx = cos(PI2f * surf_freq * Time::currentTime * 1.0f) * 20.0f * DEG_2_RADf;
    float surf_roty = cos(PI2f * surf_freq * Time::currentTime * 1.0f) * 20.0f * DEG_2_RADf;
    // float surf_rotz = sin(PI2f * surf_freq * Time::currentTime * 2.0f) * 60.0f * DEG_2_RADf;

    Eigen::Affine3f surfTransform = Eigen::Affine3f::Identity();
    surfTransform.rotate(Eigen::AngleAxisf(surf_roty, Eigen::Vector3f::UnitY()));
    surfTransform.rotate(Eigen::AngleAxisf(surf_rotx, Eigen::Vector3f::UnitX()));
    surfTransform.rotate(Eigen::AngleAxisf(surf_rotz, Eigen::Vector3f::UnitZ()));
    surfTransform.translate(Eigen::Vector3f(surf_dx, surf_dy, surf_dz));
    Eigen::Affine3f surfTransformInverse = surfTransform.inverse();

    bool all_ik_good = true;
    float angles_out[4][4];
    
    for(int i = 0; i < 4; i++){

        Leg* leg = robot->legs[i];

        Eigen::Vector3f footPosTransformed = stancePosFeetRelative[i];
        footPosTransformed = leg->hipTransform * footPosTransformed;
        // footPosTransformed = pivotTransformInverse * footPosTransformed;
        footPosTransformed = surfTransformInverse * footPosTransformed;
        // footPosTransformed = pivotTransform * footPosTransformed;
        footPosTransformed = leg->hipTransformInverse * footPosTransformed;

        bool result = leg->IKSearch(footPosTransformed, stancePhi, angles_out[i], anglesOld[i]);
        
        if(!result){
            all_ik_good = false;
            break;
        }
    }

    if(!all_ik_good){
        LogDebug("SurferBrain", "inverse kinematics search failed");
    }

    if(all_ik_good){

        int k = 0;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                float angle = angles_out[i][j];
                assert(!isnan(angle));
                robot->legs[i]->joints[j]->SetTargetAngle(angle);
            }
        }
        robot->MoveJointsToTargetSync(Time::fixedDeltaTime);
    }
}

void SurferBrain::Destroy(){

}

void SurferBrain::SetStance(float xy, float z, float hipAngle, float phi){
    this->stancePosFeetRelative[0] = Eigen::Vector3f(xy * cos(+hipAngle), xy * sin(+hipAngle), z);
    this->stancePosFeetRelative[1] = Eigen::Vector3f(xy * cos(-hipAngle), xy * sin(-hipAngle), z);
    this->stancePosFeetRelative[2] = Eigen::Vector3f(xy * cos(+hipAngle), xy * sin(+hipAngle), z);
    this->stancePosFeetRelative[3] = Eigen::Vector3f(xy * cos(-hipAngle), xy * sin(-hipAngle), z);
    this->stancePhi = phi; 
}

void SurferBrain::SetPivot(const Eigen::Vector3f& pivot){
    pivotTransform = Eigen::Affine3f::Identity();
    pivotTransform.translate(pivot);
    pivotTransformInverse = pivotTransform.inverse();
}

}