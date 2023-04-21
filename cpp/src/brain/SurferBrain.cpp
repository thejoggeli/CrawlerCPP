
#include "SurferBrain.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include <cmath>
#include "math/Mathf.h"
#include <cassert>
#include "core/Log.h"
#include "core/Time.h"
#include "remote/ClientManager.h"
#include "remote/Client.h"

using namespace std;

namespace Crawler {


SurferBrain::SurferBrain() : Brain() {
}

void SurferBrain::Init(){

    // stance from measured angles
    Leg* leg = robot->legs[0];
    float angles[4];
    leg->GetJointsCurrentTargetAngles(angles);
    leg->FKJoints(angles);
    const Eigen::Vector3f& footPosition = leg->fkJointsResult.footPosition;
    float hipAngle = angles[0];
    float stanceXY = sqrt(footPosition[0]*footPosition[0]+footPosition[1]*footPosition[1]);
    float stanceZ = footPosition[2];
    float stancePhi = 0.0f * DEG_2_RADf;
    SetStance(stanceXY, stanceZ, hipAngle, stancePhi);

}

void SurferBrain::Update(){

    auto& clients = ClientManager::GetAllCients();
    Client* client = nullptr;

    if(clients.size() > 0){
        client = clients[0].get();
    }

    if(client){

        if(client->OnKeyDown(GamepadKey::A)){
            surfMode = (surfMode+1)%3;
            LogInfo("SurferBrain", iLog << "Surf Mode is now " << surfMode);
        }

    }
    

}

void SurferBrain::FixedUpdate(){

    auto& clients = ClientManager::GetAllCients();
    Client* client = nullptr;

    if(clients.size() > 0){
        client = clients[0].get();
    }

    if(client){

        // LogDebug("SurferBrain", "has client");

        Eigen::Vector2f input = client->GetJoystickPosition(GamepadKey::LeftJoystick);

        if(surfMode == 0){

            float rx = input[0] * 20.0f * DEG_2_RADf;
            float ry = input[1] * 20.0f * DEG_2_RADf;
            // LogDebug("SurferBrain", iLog << "rx=" << rx << ", ry=" << ry);
            SetBodyRotationTarget(rx, ry, 0.0f);

            // move base up and down
            if(client->IsKeyDown(GamepadKey::Down)){
                SetBodyTranslationTarget(bodyTranslationTarget[0], bodyTranslationTarget[1], bodyTranslationTarget[2] - 0.05f * Time::fixedDeltaTime);
            } else if(client->IsKeyDown(GamepadKey::Up)){
                SetBodyTranslationTarget(bodyTranslationTarget[0], bodyTranslationTarget[1], bodyTranslationTarget[2] + 0.05f * Time::fixedDeltaTime);
            }

            // change target phi angles
            if(client->IsKeyDown(GamepadKey::Left)){
                stancePhi -= 25.0f * Time::fixedDeltaTime * DEG_2_RADf;
            } else if(client->IsKeyDown(GamepadKey::Right)){
                stancePhi += 25.0f * Time::fixedDeltaTime * DEG_2_RADf;
            }

        } else if(surfMode == 1) {

            float dx = input[1] *   0.05f;
            float dy = -input[0] * 0.05f;
            // LogDebug("SurferBrain", iLog << "dx=" << dx << ", dy=" << dy);
            SetBodyTranslationTarget(dx, dy, bodyTranslationTarget[2]);

            // narrow or widen stance 
            if(client->IsKeyDown(GamepadKey::Down)){
                SetStance(stanceXY - 0.05f * Time::fixedDeltaTime, stanceZ, stanceHipAngle, stancePhi);
            } else if(client->IsKeyDown(GamepadKey::Up)){
                SetStance(stanceXY + 0.05f * Time::fixedDeltaTime, stanceZ, stanceHipAngle, stancePhi);
            }

            // change stance hip angle 
            if(client->IsKeyDown(GamepadKey::Left)){
                SetStance(stanceXY, stanceZ, stanceHipAngle - 25.0f * Time::fixedDeltaTime * DEG_2_RADf, stancePhi);
            } else if(client->IsKeyDown(GamepadKey::Right)){
                SetStance(stanceXY, stanceZ, stanceHipAngle + 25.0f * Time::fixedDeltaTime * DEG_2_RADf, stancePhi);
            }

        } else if(surfMode == 2){
            // float rz = input[0] * 30.0f * DEG_2_RADf;
            // SetbodyRotationTarget(rx, ry, 0.0f);
            // SetbodyRotationTarget(0.0f, 0.0f, rz);
            float dx = 0.5f;
            float dy = -input[0] * 0.5f;
            float dz = -input[1] * 0.5f;
            float rx = bodyRotationTarget[0];
            float ry = atan2(dz, dx);
            float rz = atan2(dy, dx);

            // change stance hip angle 
            if(client->IsKeyDown(GamepadKey::Left)){
                rx -= Time::fixedDeltaTime * DEG_2_RADf * 20.0f;
            } else if(client->IsKeyDown(GamepadKey::Right)){
                rx += Time::fixedDeltaTime * DEG_2_RADf * 20.0f;
            }
            
            SetBodyRotationTarget(rx, ry, rz);

        }

    }

    float footAngles[4] = {stancePhi, stancePhi, stancePhi, stancePhi};

    // AutoSurf();
    SurfTowardsTarget();
    UpdateBodyTransform();
    ApplyPoseToJoints(footPositions, footAngles, bodyTransform, Time::fixedDeltaTime);

}

void SurferBrain::AutoSurf(){

    float surfFrequency = 0.2;

    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;

    // float dx = cos(PI2f * surf_freq * Time::currentTime * 1.0f) * 0.03f;
    // float dy = sin(PI2f * surf_freq * Time::currentTime * 1.0f) * 0.03f;
    // float dz = cos(PI2f * surf_freq * Time::currentTime * 1.0f) * 0.05f;

    // float rx = 0.0f;
    // float ry = 0.0f;
    float rz = 0.0f;

    float rx = cos(PI2f * surfFrequency * Time::currentTime * 1.0f) * 20.0f * DEG_2_RADf;
    float ry = sin(PI2f * surfFrequency * Time::currentTime * 1.0f) * 20.0f * DEG_2_RADf;
    // float rz = sin(PI2f * surf_freq * Time::currentTime * 2.0f) * 60.0f * DEG_2_RADf;

    SetBodyTranslationTarget(dx, dy, dz);
    SetBodyRotationTarget(rx, ry, rz);
    
}

void SurferBrain::UpdateBodyTransform(){
    bodyTransform.setIdentity();
    bodyTransform.rotate(Eigen::AngleAxisf(bodyRotation[1], Eigen::Vector3f::UnitY()));
    bodyTransform.rotate(Eigen::AngleAxisf(bodyRotation[0], Eigen::Vector3f::UnitX()));
    bodyTransform.rotate(Eigen::AngleAxisf(bodyRotation[2], Eigen::Vector3f::UnitZ()));
    bodyTransform.translate(bodyTranslation);
}

void SurferBrain::Destroy(){

}

void SurferBrain::SetStance(float xy, float z, float hipAngle, float phi){
    this->stanceXY = xy;
    this->stanceZ = z;
    this->stanceHipAngle = hipAngle;
    this->stancePhi = phi; 
    this->footPositions[0] = robot->legs[0]->hipTransform * Eigen::Vector3f(xy * cos(+hipAngle), xy * sin(+hipAngle), z);
    this->footPositions[1] = robot->legs[1]->hipTransform * Eigen::Vector3f(xy * cos(-hipAngle), xy * sin(-hipAngle), z);
    this->footPositions[2] = robot->legs[2]->hipTransform * Eigen::Vector3f(xy * cos(+hipAngle), xy * sin(+hipAngle), z);
    this->footPositions[3] = robot->legs[3]->hipTransform * Eigen::Vector3f(xy * cos(-hipAngle), xy * sin(-hipAngle), z);
}

void SurferBrain::SetPivot(const Eigen::Vector3f& pivot){
    pivotTransform = Eigen::Affine3f::Identity();
    pivotTransform.translate(pivot);
    pivotTransformInverse = pivotTransform.inverse();
}

void SurferBrain::SetbodyTranslationSpeed(float speed){
    this->bodyTranslationSpeed = speed;
}

void SurferBrain::SetbodyRotationSpeed(float speed){
    this->bodyRotationSpeed = speed;
}

void SurferBrain::SetBodyTranslationTarget(float x, float y, float z){
    SetbodyTranslationTarget(Eigen::Vector3f(x, y, z));
}

void SurferBrain::SetbodyTranslationTarget(const Eigen::Vector3f& translation){
    bodyTranslationStart = bodyTranslation;
    bodyTranslationTarget = translation;
    bodyTranslationDelta = bodyTranslationTarget - bodyTranslationStart;
    float norm = bodyTranslationDelta.norm();
    bodyTranslationTime = 0.0f;
    bodyTranslationDuration = norm/bodyTranslationSpeed;
}

void SurferBrain::SetBodyRotationTarget(float x, float y, float z){
    SetbodyRotationTarget(Eigen::Vector3f(x, y, z));
}

void SurferBrain::SetbodyRotationTarget(const Eigen::Vector3f& rotation){
    bodyRotationStart = bodyRotation;
    bodyRotationTarget = rotation;
    bodyRotationDelta = bodyRotationTarget - bodyRotationStart;
    float norm = bodyRotationDelta.norm();
    bodyRotationTime = 0.0f;
    bodyRotationDuration = norm/bodyRotationSpeed;
}

void SurferBrain::SurfTowardsTarget(){

    // move translation towards target translation
    if(bodyTranslationTime < bodyTranslationDuration){
        bodyTranslationTime = min(bodyTranslationDuration, bodyTranslationTime + Time::fixedDeltaTime);
        float t = bodyTranslationTime / bodyTranslationDuration;
        bodyTranslation = bodyTranslationStart + bodyTranslationDelta * t;
    }   

    // move rotation towards target rotation
    if(bodyRotationTime < bodyRotationDuration){
        bodyRotationTime = min(bodyRotationDuration, bodyRotationTime + Time::fixedDeltaTime);
        float t = bodyRotationTime / bodyRotationDuration;
        bodyRotation = bodyRotationStart + bodyRotationDelta * t;
    }

}

}