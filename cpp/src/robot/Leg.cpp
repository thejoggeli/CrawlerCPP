#include "Leg.h"
#include <cmath>
#include <limits>
#include <cassert>
#include <iostream>
#include "math/Mathf.h"
#include "Robot.h"
#include "core/Log.h"
#include "parts/WeightSensor.h"
#include "parts/DistanceSensor.h"
#include "parts/MuxI2C.h"

using namespace std;

// #define DEBUG_LEG_IK

namespace Crawler {

Leg::Leg(Robot* robot, unsigned int id, const std::string& name){

    this->id = id;
    this->robot = robot;
    this->name = name;
    
    // create weight sensor
    this->weightSensor = new WeightSensor();

    // create distance sensor
    this->distanceSensor = new DistanceSensor();

    // create joints
    joints.push_back(new Joint(this, JointType::H0));
    joints.push_back(new Joint(this, JointType::K1));
    joints.push_back(new Joint(this, JointType::K2));
    joints.push_back(new Joint(this, JointType::K3));

    // ik search config
    const int numPhiVals = 251;
    const float phiMin = -45.0f * DEG_2_RADf;
    const float phiMax = +45.0f * DEG_2_RADf;
    IKSearchConfig(numPhiVals, phiMin, phiMax);
    
}

Leg::~Leg(){
    if(weightSensor){
        delete weightSensor;
    }
    if(distanceSensor){
        delete distanceSensor;
    }
}

bool Leg::InitDistanceSensor(MuxI2C* mux, int channel){
    return true;
    // return this->distanceSensor->Init(mux, channel);
}

bool Leg::InitWeightSensor(MuxI2C* mux, int channel){
    return true;
    // return this->weightSensor->Init(mux, channel);
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
        if(v > 1.0f && v < 1.0001f){
            v = 1.0f;
        } else if(v < -1.0f && v > -1.0001f){
            v = -1.0f;
        } else {
            #ifdef DEBUG_LEG_IK
                LogDebug("Leg", iLog << "v out of limits " << v);
            #endif
            return false;
        }
    }
    
    float a2 = -acos(v); 
    a2 = Mathf::AngleToSymmetric(a2);
    if(a2 < joints[2]->limitMin || a2 > joints[2]->limitMax){
        #ifdef DEBUG_LEG_IK
            LogDebug("Leg", iLog << "a2 out of limits " << a2 * RAD_2_DEGf);
        #endif
        return false;
    }

    float a1 = atan2(z, xy) - atan2(L2 * sin(a2), L1 + L2 * cos(a2));
    a1 = Mathf::AngleToSymmetric(a1);
    if(a1 < joints[1]->limitMin || a1 > joints[1]->limitMax){
        #ifdef DEBUG_LEG_IK
            LogDebug("Leg", iLog << "a  1 out of limits " << a1 * RAD_2_DEGf);
        #endif
        return false;
    }

    float a3 = phi - a1 - a2 - PIf * 0.5f;
    a3 = Mathf::AngleToSymmetric(a3);
    if(a3 < joints[3]->limitMin || a3 > joints[3]->limitMax){
        #ifdef DEBUG_LEG_IK
            LogDebug("Leg", iLog << "a3 out of limits " << a3 * RAD_2_DEGf);
        #endif
        return false;
    }

    float a0 = atan2(Q[1], Q[0]);
    a0 = Mathf::AngleToSymmetric(a0);
    if(a0 < joints[0]->limitMin || a0 > joints[0]->limitMax){
        #ifdef DEBUG_LEG_IK
            LogDebug("Leg", iLog << "a0 out of limits " << a0 * RAD_2_DEGf);
        #endif
        return false;
    }
    
    angles_out[0] = a0;
    angles_out[1] = -a1;
    angles_out[2] = -a2;
    angles_out[3] = -a3;
    
    return true;

}

void Leg::IKSearchConfig(int numPhiVals, float phiMin, float phiMax){

    // clear search phi list
    ikSearchPhiValues.clear();

    // fill search phi list with new values
    float phiStep = (phiMax - phiMin) / (float)(numPhiVals-1);
    for(int i = 0; i < numPhiVals; i++){
        float phi = phiMin + phiStep * (float)i;
        ikSearchPhiValues.push_back(phi);
    }

}

bool Leg::IKSearch(const Eigen::Vector3f& Q, float phiTarget, float phiOld, float anglesOld[4], float anglesOut[4]){

    bool hasResult = false;
    float bestLoss = numeric_limits<float>::infinity();
    float bestPhi = 0.0f;
    float anglesTest[4];

    // test IK for all phi values
    for(float phiTest : ikSearchPhiValues){
        bool result = IKExact(Q, phiTest, anglesTest);
        if(result){
            float lossVal = IKLoss(phiTarget, phiOld, phiTest, anglesOld, anglesTest);
            if(lossVal < bestLoss){
                bestLoss = lossVal;
                bestPhi = phiTest;
                hasResult = true;
            }
        }
    }

    if(hasResult){
        bool result = IKExact(Q, bestPhi, anglesOut);
        assert(result == true);
    }

    return hasResult;
    
}

float Leg::IKLoss(float phiTarget, float phiOld, float phiNew, float anglesOld[4], float anglesNew[4]){

    const float phiErrorWeight = 10.0f; // phi deviation from target value
    const float phiDeltaWeight = 0.0f; // phi change since last frame
    const float angleDeltaWeight = 0.0f; // joint angles change since last frame 

    float loss = 0.0f;
    
    // error phi
    float phiError = phiTarget - phiNew;
    loss += (phiError*phiError)*phiErrorWeight;

    // delta phi
    if(phiDeltaWeight != 0.0f){
        float phiDelta = phiNew - phiOld;
        loss += phiDelta*phiDelta*phiDeltaWeight;
    }

    // delta angle
    if(angleDeltaWeight != 0.0f){

        // find max delta angle
        float angleDeltaMax = 0.0f;
        for(int i = 0; i < 4; i++){
            float angleDelta = abs(anglesOld[i] - anglesNew[i]);
            if(angleDelta > angleDeltaMax){
                angleDeltaMax = angleDelta;
            }
        }

        // add max angle loss
        loss += (angleDeltaMax*angleDeltaMax)*angleDeltaWeight;

    }

    // return final loss
    return loss;

}

float Leg::FKPhi(float angles[4]){
    return PIf*0.5f - (angles[1] + angles[2] + angles[3]);
}

void Leg::FKJoints(float angles[4]){

    fkJointsResult.jointPositions[0][0] = 0.0f;
    fkJointsResult.jointPositions[0][1] = 0.0f;
    fkJointsResult.jointPositions[0][2] = 0.0f;

    float cos_a0 = cos(angles[0]);
    float sin_a0 = sin(angles[0]);

    fkJointsResult.jointPositions[1][0] = joints[0]->length * cos_a0;
    fkJointsResult.jointPositions[1][1] = joints[0]->length * sin_a0;
    fkJointsResult.jointPositions[1][2] = 0.0f;

    float angleCum = 0.0f;
    for(int i = 1; i < 4; i++){
        angleCum += angles[i];
        float cos_aCum = cos(angleCum);
        fkJointsResult.jointPositions[i+1][0] = fkJointsResult.jointPositions[i][0] + cos_a0 * joints[i]->length * cos_aCum;
        fkJointsResult.jointPositions[i+1][1] = fkJointsResult.jointPositions[i][1] + sin_a0 * joints[i]->length * cos_aCum;
        fkJointsResult.jointPositions[i+1][2] = fkJointsResult.jointPositions[i][2] - sin(angleCum) * joints[i]->length;
    }

}

void Leg::GetJointsCurrentTargetAngles(float angles[4]){
    for(int i = 0; i < joints.size(); i++){
        angles[i] = joints[i]->currentTargetAngle;
    }
}

void Leg::GetJointsMeasuredAngles(float angles[4]){
    for(int i = 0; i < joints.size(); i++){
        angles[i] = joints[i]->measuredAngle;
    }
}

void Leg::GetJointsLastTargetAngles(float angles[4]){
    for(int i = 0; i < joints.size(); i++){
        angles[i] = joints[i]->lastTargetAngle;
    }
}

void Leg::TorqueOn(bool buffer){
    for(Joint* joint : joints){
        joint->TorqueOn(buffer);
    }
}

void Leg::TorqueOff(bool buffer){
    for(Joint* joint : joints){
        joint->TorqueOff(buffer);
    }
}

bool Leg::ReadMeasuredDistance(bool buffer){
    uint16_t proximity = distanceSensor->ReadProximity();
    float distance = (float)proximity;
    if(buffer){
        measuredDistance.BufferValue(distance);
    } else {
        measuredDistance.SetValue(distance);
    }
    return true;
}

bool Leg::ReadMeasuredWeight(bool buffer){
    return true;
}


}