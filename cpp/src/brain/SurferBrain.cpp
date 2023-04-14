
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
        if(client->OnKeyDown(KeyCode::A)){
            surfMode = (surfMode+1)%3;
        }
        if(client->OnKeyDown(KeyCode::B)){
            stanceMode = (stanceMode+1)%2;
        }

        if(stanceMode == 0){

            // move base up and down
            if(client->IsKeyDown(KeyCode::Down)){
                SetSurfTranslationTarget(surfTranslationTarget[0], surfTranslationTarget[1], surfTranslationTarget[2] - 0.05f * Time::deltaTime);
            } else if(client->IsKeyDown(KeyCode::Up)){
                SetSurfTranslationTarget(surfTranslationTarget[0], surfTranslationTarget[1], surfTranslationTarget[2] + 0.05f * Time::deltaTime);
            }

            // change target phi angles
            if(client->IsKeyDown(KeyCode::Left)){
                stancePhi -= 25.0f * Time::deltaTime * DEG_2_RADf;
            } else if(client->IsKeyDown(KeyCode::Right)){
                stancePhi += 25.0f * Time::deltaTime * DEG_2_RADf;
            }

        } else if(stanceMode == 1){

            // narrow or widen stance 
            if(client->IsKeyDown(KeyCode::Down)){
                SetStance(stanceXY - 0.05f * Time::deltaTime, stanceZ, stanceHipAngle, stancePhi);
            } else if(client->IsKeyDown(KeyCode::Up)){
                SetStance(stanceXY + 0.05f * Time::deltaTime, stanceZ, stanceHipAngle, stancePhi);
            }

            // change stance hip angle 
            if(client->IsKeyDown(KeyCode::Left)){
                SetStance(stanceXY, stanceZ, stanceHipAngle - 25.0f * Time::deltaTime * DEG_2_RADf, stancePhi);
            } else if(client->IsKeyDown(KeyCode::Right)){
                SetStance(stanceXY, stanceZ, stanceHipAngle + 25.0f * Time::deltaTime * DEG_2_RADf, stancePhi);
            }

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

        Eigen::Vector2f input = client->GetJoystickPosition(KeyCode::LeftJoystick);

        if(surfMode == 0){
            float rx = input[0] * 20.0f * DEG_2_RADf;
            float ry = input[1] * 20.0f * DEG_2_RADf;
            // LogDebug("SurferBrain", iLog << "rx=" << rx << ", ry=" << ry);
            SetSurfRotationTarget(rx, ry, 0.0f);
        } else if(surfMode == 1) {
            float dx = input[1] *   0.05f;
            float dy = -input[0] * 0.05f;
            // LogDebug("SurferBrain", iLog << "dx=" << dx << ", dy=" << dy);
            SetSurfTranslationTarget(dx, dy, surfTranslationTarget[2]);
        } else if(surfMode == 2){
            float rz = input[0] * 30.0f * DEG_2_RADf;
            // SetSurfRotationTarget(rx, ry, 0.0f);
            SetSurfRotationTarget(0.0f, 0.0f, rz);
        }

    }

    // AutoSurf();
    SurfTowardsTarget();
    UpdateSurfTransform();
    ApplySurfToJoints();

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

    SetSurfTranslationTarget(dx, dy, dz);
    SetSurfRotationTarget(rx, ry, rz);
    
}

bool SurferBrain::ApplySurfToJoints(){

    // LogDebug("SurferBrain", iLog << stancePhi * RAD_2_DEGf);

    float outAngles[4][4];

    bool ikSuccess[4];
    bool ikSuccessAll = true;
    
    for(int i = 0; i < 4; i++){

        Leg* leg = robot->legs[i];

        float oldAngles[4] = {
            leg->joints[0]->lastTargetAngle,
            leg->joints[1]->lastTargetAngle,
            leg->joints[2]->lastTargetAngle,
            leg->joints[3]->lastTargetAngle,
        };

        float oldPhi = leg->FKPhi(oldAngles);

        Eigen::Vector3f footPosTransformed = stancePosFeetRelative[i];
        footPosTransformed = leg->hipTransform * footPosTransformed;
        // footPosTransformed = pivotTransformInverse * footPosTransformed;
        footPosTransformed = surfTransformInverse * footPosTransformed;
        // footPosTransformed = pivotTransform * footPosTransformed;
        footPosTransformed = leg->hipTransformInverse * footPosTransformed;

        bool result = leg->IKSearch(footPosTransformed, stancePhi, oldPhi, oldAngles, outAngles[i]);

        ikSuccess[i] = result;        
        if(!result){
            ikSuccessAll = false;
        }

    }

    if(ikSuccessAll){
        // IK successful
        // set new target postion for all servos
        int k = 0;
        for(int i = 0; i < 4; i++){
            for(int j = 0; j < 4; j++){
                float angle = outAngles[i][j];
                assert(!isnan(angle));
                robot->legs[i]->joints[j]->SetTargetAngle(angle);
            }
        }
        // move servos to target position sync
        robot->MoveJointsToTargetSync(Time::fixedDeltaTime);
    } else {
        // IK failed
        LogDebug("SurferBrain", "inverse kinematics search failed");
    }

    // set IK servo colors
    for(int i = 0; i < 4; i++){
        Leg* leg = robot->legs[i];
        int r, g, b, w;
        if(ikSuccess[i]){
            r = 0; g = 1; b = 1; w = 0;
        } else {
            r = 1; g = 1; b = 0; w = 0;
        }
        for(Joint* joint : leg->joints){
            joint->SetServoLedColor(r, g, b, w);
        }
    }
    
    return ikSuccessAll;
    
}

void SurferBrain::Destroy(){

}

void SurferBrain::SetStance(float xy, float z, float hipAngle, float phi){
    this->stanceXY = xy;
    this->stanceZ = z;
    this->stanceHipAngle = hipAngle;
    this->stancePhi = phi; 
    this->stancePosFeetRelative[0] = Eigen::Vector3f(xy * cos(+hipAngle), xy * sin(+hipAngle), z);
    this->stancePosFeetRelative[1] = Eigen::Vector3f(xy * cos(-hipAngle), xy * sin(-hipAngle), z);
    this->stancePosFeetRelative[2] = Eigen::Vector3f(xy * cos(+hipAngle), xy * sin(+hipAngle), z);
    this->stancePosFeetRelative[3] = Eigen::Vector3f(xy * cos(-hipAngle), xy * sin(-hipAngle), z);
}

void SurferBrain::SetPivot(const Eigen::Vector3f& pivot){
    pivotTransform = Eigen::Affine3f::Identity();
    pivotTransform.translate(pivot);
    pivotTransformInverse = pivotTransform.inverse();
}

void SurferBrain::SetSurfTranslationSpeed(float speed){
    this->surfTranslationSpeed = speed;
}

void SurferBrain::SetSurfRotationSpeed(float speed){
    this->surfRotationSpeed = speed;
}

void SurferBrain::SetSurfTranslationTarget(float x, float y, float z){
    SetSurfTranslationTarget(Eigen::Vector3f(x, y, z));
}

void SurferBrain::SetSurfTranslationTarget(const Eigen::Vector3f& translation){
    surfTranslationStart = surfTranslation;
    surfTranslationTarget = translation;
    surfTranslationDelta = surfTranslationTarget - surfTranslationStart;
    float norm = surfTranslationDelta.norm();
    surfTranslationTime = 0.0f;
    surfTranslationDuration = norm/surfTranslationSpeed;
}

void SurferBrain::SetSurfRotationTarget(float x, float y, float z){
    SetSurfRotationTarget(Eigen::Vector3f(x, y, z));
}

void SurferBrain::SetSurfRotationTarget(const Eigen::Vector3f& rotation){
    surfRotationStart = surfRotation;
    surfRotationTarget = rotation;
    surfRotationDelta = surfRotationTarget - surfRotationStart;
    float norm = surfRotationDelta.norm();
    surfRotationTime = 0.0f;
    surfRotationDuration = norm/surfRotationSpeed;
}

void SurferBrain::SurfTowardsTarget(){

    // move translation towards target translation
    if(surfTranslationTime < surfTranslationDuration){
        surfTranslationTime = min(surfTranslationDuration, surfTranslationTime + Time::fixedDeltaTime);
        float t = surfTranslationTime / surfTranslationDuration;
        surfTranslation = surfTranslationStart + surfTranslationDelta * t;
    }   

    // move rotation towards target rotation
    if(surfRotationTime < surfRotationDuration){
        surfRotationTime = min(surfRotationDuration, surfRotationTime + Time::fixedDeltaTime);
        float t = surfRotationTime / surfRotationDuration;
        surfRotation = surfRotationStart + surfRotationDelta * t;
    }

}

void SurferBrain::UpdateSurfTransform(){
    surfTransform.setIdentity();
    surfTransform.rotate(Eigen::AngleAxisf(surfRotation[1], Eigen::Vector3f::UnitY()));
    surfTransform.rotate(Eigen::AngleAxisf(surfRotation[0], Eigen::Vector3f::UnitX()));
    surfTransform.rotate(Eigen::AngleAxisf(surfRotation[2], Eigen::Vector3f::UnitZ()));
    surfTransform.translate(surfTranslation);
    surfTransformInverse = surfTransform.inverse();
}

}