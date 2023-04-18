#include "GaitBrain.h"
#include "gait/Gait.h"
#include "gait/GaitFrame.h"
#include "core/Time.h"
#include "core/Log.h"
#include "remote/ClientManager.h"
#include "remote/Client.h"
#include "robot/Robot.h"
#include "robot/Joint.h"
#include "robot/Leg.h"
#include "math/Mathf.h"

namespace Crawler {

GaitBrain::GaitBrain() : Brain() {

}

GaitBrain::~GaitBrain(){
    if(gait != nullptr){
        delete gait;
    }
}

void GaitBrain::Init(){
    
    gait = new Gait();
    
    GaitFrame frame;

    for(int i = 0; i < 4; i++){
        Leg* leg = robot->legs[i];
        float angles[4];
        leg->GetJointsCurrentTargetAngles(angles);
        leg->FKJoints(angles);
        frame.footPositions[i] = leg->hipTransform * leg->fkJointsResult.footPosition;
    }

    float dxy = 0.05f;

    frame.bodyPosition = {dxy, 0.0f, 0.0f};
    frame.bodyRotation = {0.0f, 0.0f, 0.0f};
    gait->AddFrame(frame);
    gait->AddTransition(1.0f);

    frame.bodyPosition = {0.0f, dxy, 0.0f};
    frame.bodyRotation = {20.0f * DEG_2_RADf, 0.0f, 0.0f};
    gait->AddFrame(frame);
    gait->AddTransition(1.0f);

    frame.bodyPosition = {-dxy, 0.0f, 0.0f};
    frame.bodyRotation = {0.0f, 0.0f, 0.0f};
    gait->AddFrame(frame);
    gait->AddTransition(1.0f);

    frame.bodyPosition = {0.0f, dxy, 0.0f};
    frame.bodyRotation = {-20.0f * DEG_2_RADf, 0.0f, 0.0f};
    gait->AddFrame(frame);
    gait->AddTransition(1.0);

}

void GaitBrain::Update(){

    auto& clients = ClientManager::GetAllCients();
    if(clients.size() > 0){
        Client* client = clients[0].get();

        if(client->OnKeyDown(KeyCode::A)){
            playing = !playing;
            LogDebug("GaitBrain", iLog << "playing=" << (int)playing);
        }

    }

}

void GaitBrain::FixedUpdate(){

    auto& clients = ClientManager::GetAllCients();
    if(clients.size() > 0){
        Client* client = clients[0].get();

        if(client->IsKeyDown(KeyCode::Up)){
            speed += Time::fixedDeltaTime;
            LogDebug("GaitBrain", iLog << "speed=" << speed);
        }
        if(client->IsKeyDown(KeyCode::Down)){
            speed -= Time::fixedDeltaTime;
            LogDebug("GaitBrain", iLog << "speed=" << speed);
        }
    }
    
    if(playing){
        gait->StepForward(Time::fixedDeltaTime * speed);

        // const Eigen::Vector3f& p = gait->bodyPosition;
        // LogInfo("GaitBrain", iLog << "bodyPosition=(" << p.x() << ", " << p.y() << ", " << p.z() << ")" );

        Eigen::Affine3f bodyTransform = Eigen::Affine3f::Identity();
        bodyTransform.rotate(Eigen::AngleAxisf(gait->bodyRotation[1], Eigen::Vector3f::UnitY()));
        bodyTransform.rotate(Eigen::AngleAxisf(gait->bodyRotation[0], Eigen::Vector3f::UnitX()));
        bodyTransform.rotate(Eigen::AngleAxisf(gait->bodyRotation[2], Eigen::Vector3f::UnitZ()));
        bodyTransform.translate(gait->bodyPosition);
        
        ApplyPoseToJoints(gait->footPositions.data(), gait->footAngles.data(), bodyTransform, Time::fixedDeltaTime);

    }

}

void GaitBrain::Destroy(){

}

}