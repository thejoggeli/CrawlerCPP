#include "WalkerBrain.h"
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

WalkerBrain::WalkerBrain() : Brain("walk") {

}

WalkerBrain::~WalkerBrain(){
    if(g_walk_1 != nullptr){
        delete g_walk_1;
    }
    if(g_walk_2 != nullptr){
        delete g_walk_2;
    }
    if(g_turn_1 != nullptr){
        delete g_turn_1;
    }
    if(g_turn_2 != nullptr){
        delete g_turn_2;
    }
}

void WalkerBrain::Init(){

    robot->Startup();

    InitGaitWalk();
    InitGaitTurn();

    for(Joint* joint: robot->jointsList){
        joint->SetServoLedColor(0, 0, 0, 0, true);
    }

}

void WalkerBrain::InitGaitWalk(){

    LogDebug("WalkerBrain", "init gait walk start");
    
    g_walk_1 = new Gait();
    g_walk_2 = new Gait();
    
    GaitFrame f1;    
    GaitFrame f2;

    // f1.bodyPosition[0] = 0.025f;
    // f2.bodyPosition[0] = 0.025f;

    // f1.bodyPosition[1] = 0.0f;
    // f2.bodyPosition[1] = 0.0f;

    // f1.bodyPosition[2] = 0.0f;
    // f2.bodyPosition[2] = 0.0f;

    float dx_far = 0.015;
    float dx_close = -0.015;
    float dy = 0.0;
    float h_floor = -0.1;
    float h_up = -0.05;

    float t0 = 0.2f;
    float t1 = 0.04f;
    float t2 = 0.04f;
    float t3 = 0.08f;

    f1.footPositions[0][0] = +dx_far;
    f1.footPositions[0][1] = +dy;
    f1.footPositions[0][2] = h_floor;

    g_walk_1->AddFrame(f1);
    g_walk_1->AddTransition(t0);

    f1.footPositions[0][0] = +dx_close;

    g_walk_1->AddFrame(f1);
    g_walk_1->AddTransition(t1);

    f1.footPositions[0][2] = h_up;

    g_walk_1->AddFrame(f1);
    g_walk_1->AddTransition(t2);

    f1.footPositions[0][0] = +dx_far;

    g_walk_1->AddFrame(f1);
    g_walk_1->AddTransition(t3);

    f2.footPositions[0][0] = -dx_close;
    f2.footPositions[0][1] = +dy;
    f2.footPositions[0][2] = h_floor;

    g_walk_2->AddFrame(f2);
    g_walk_2->AddTransition(t0);

    f2.footPositions[0][0] = -dx_far;

    g_walk_2->AddFrame(f2);
    g_walk_2->AddTransition(t1);

    f2.footPositions[0][2] = h_up;

    g_walk_2->AddFrame(f2);
    g_walk_2->AddTransition(t2);

    f2.footPositions[0][0] = -dx_close;

    g_walk_2->AddFrame(f2);
    g_walk_2->AddTransition(t3);

}

void WalkerBrain::InitGaitTurn(){

    LogDebug("WalkerBrain", "init gait turn start");
    
    g_turn_1 = new Gait();
    g_turn_2 = new Gait();
    
    GaitFrame f1;
    GaitFrame f2;

    float h_floor = -0.1;
    float h_up = -0.05;
    float radius = sqrtf(dx*dx+dy*dy);

    float a_from = -5.0f * DEG_2_RADf;
    float a_to   = +5.0f * DEG_2_RADf;

    float t0 = 0.2f;
    float t1 = 0.04f;
    float t2 = 0.04f;
    float t3 = 0.08f;

    float t_tot = t0 + t1 + t2 + t3;

    float dt = 0.005f;

    for(float t = 0; t < t0; t += dt){
        float a = PIf * 0.25f + t/t0 * (a_to - a_from) + a_from;
        f1.footPositions[0][0] = cosf(a) * radius;
        f1.footPositions[0][1] = sinf(a) * radius;
        f1.footPositions[0][2] = h_floor;
        g_turn_1->AddFrame(f1);
        g_turn_2->AddFrame(f1);
        g_turn_1->AddTransition(dt);
        g_turn_2->AddTransition(dt);
    }
    
    f1.footPositions[0][0] = cosf(PIf * 0.25f + a_to) * radius;
    f1.footPositions[0][1] = sinf(PIf * 0.25f + a_to) * radius;
    f1.footPositions[0][2] = h_floor;
    g_turn_1->AddFrame(f1);
    g_turn_2->AddFrame(f1);
    g_turn_1->AddTransition(t1);
    g_turn_2->AddTransition(t1);
    
    f1.footPositions[0][2] = h_up;
    g_turn_1->AddFrame(f1);
    g_turn_2->AddFrame(f1);
    g_turn_1->AddTransition(dt);
    g_turn_2->AddTransition(dt);
    
    for(float t = 0; t < t2; t += dt){
        float a = PIf * 0.25f + t/t2 * (a_from - a_to) + a_to;
        f1.footPositions[0][0] = cosf(a) * radius;
        f1.footPositions[0][1] = sinf(a) * radius;
        f1.footPositions[0][2] = h_up;
        g_turn_1->AddFrame(f1);
        g_turn_2->AddFrame(f1);
        g_turn_1->AddTransition(dt);
        g_turn_2->AddTransition(dt);
    }

    f1.footPositions[0][0] = cosf(PIf * 0.25f + a_from) * radius;
    f1.footPositions[0][1] = sinf(PIf * 0.25f + a_from) * radius;
    f1.footPositions[0][2] = h_up;
    g_turn_1->AddFrame(f1);
    g_turn_2->AddFrame(f1);
    g_turn_1->AddTransition(t3);
    g_turn_2->AddTransition(t3);

    LogDebug("WalkerBrain", "init gait turn complete");
    
}

void WalkerBrain::Update(){

}

void WalkerBrain::FixedUpdate(){

    auto& clients = ClientManager::GetAllCients();
    Client* client = nullptr;

    if(clients.size() > 0){
        client = clients[0].get();
    }

    if(client){
    
        Eigen::Vector2f input = client->GetJoystickPosition(GamepadKey::LeftJoystick);

        // LogDebug("WalkerBrain", iLog << input[0] << " / " << input[1]);

        float jx = -input[1];
        float jy = -input[0];
        float jlen = sqrtf(jx*jx+jy*jy);

        float turn = 0.0f;
        // change target phi angles
        if(client->IsKeyDown(GamepadKey::Left)){
            turn = -1.0f;
        } else if(client->IsKeyDown(GamepadKey::Right)){
            turn = +1.0f;
        }

        float elevation = 0.0f;
        // change target phi angles
        if(client->IsKeyDown(GamepadKey::Up)){
            elevation = +1.0f;
        } else if(client->IsKeyDown(GamepadKey::Down)){
            elevation = -1.0f;
        }

        if(elevation != 0.0f){
            dz += elevation * Time::fixedDeltaTime * 0.1f;
            const float minz = 0.0f;
            const float maxz = +0.1f;
            if(dz < minz){
                dz = minz;
            } else if(dz > maxz){
                dz = maxz;
            }
        }

        if(jlen > 0.01f){
            float jangle = atan2f(jy,jx);
            float movespeed = 0.5f + jlen*0.5f;

            g_walk_1->StepForward(Time::fixedDeltaTime * movespeed);

            float t2 = g_walk_1->time + g_walk_1->duration*0.5;
            if(t2 >= g_walk_1->duration){ 
                t2 -= g_walk_1->duration;
            }

            g_walk_2->StepTo(t2);

            // LogDebug("WalkerBrain", iLog << g_walk_1->currFrameIndex << "-" << g_walk_1->nextFrameIndex << " // " << g_walk_2->currFrameIndex << "-" << g_walk_2->nextFrameIndex);
                
            Eigen::Affine3f bodyTransform = Eigen::Affine3f::Identity();
            bodyTransform.rotate(Eigen::AngleAxisf(g_walk_1->bodyRotation[1], Eigen::Vector3f::UnitY()));
            bodyTransform.rotate(Eigen::AngleAxisf(g_walk_1->bodyRotation[0], Eigen::Vector3f::UnitX()));
            bodyTransform.rotate(Eigen::AngleAxisf(g_walk_1->bodyRotation[2], Eigen::Vector3f::UnitZ()));

            Eigen::Vector3f bodyPosition = {0.025f, 0.0f, 0.0f};
            bodyPosition[0] = 0.01f * cosf(jangle);
            bodyPosition[1] = 0.01f * sinf(jangle);                
            bodyTransform.translate(bodyPosition);
            
            Eigen::Vector3f footPositions[4] = {
                g_walk_1->footPositions[0],
                g_walk_2->footPositions[0],
                g_walk_1->footPositions[0],
                g_walk_2->footPositions[0],
            }; 

            float footAngles[4] = {
                g_walk_1->footAngles[0],
                g_walk_2->footAngles[1],
                g_walk_1->footAngles[2],
                g_walk_2->footAngles[3],
            };

            for(int i = 0; i < 4; i++){

                float fx = footPositions[i][0];
                float fy = footPositions[i][1];
                float flen = sqrtf(fx*fx+fy*fy);
                float fangle = atan2f(fy,fx);

                float a = fangle + jangle;

                float x = cosf(a) * flen;
                float y = sinf(a) * flen;

                footPositions[i][0] = x;
                footPositions[i][1] = y;

            }
            
            footPositions[0][0] += dx;
            footPositions[0][1] += dy;
            footPositions[0][2] -= dz;

            footPositions[1][0] -= dx;
            footPositions[1][1] += dy;
            footPositions[1][2] -= dz;

            footPositions[2][0] -= dx;
            footPositions[2][1] -= dy;
            footPositions[2][2] -= dz;

            footPositions[3][0] += dx;
            footPositions[3][1] -= dy;
            footPositions[3][2] -= dz;
            
            ApplyPoseToJoints(footPositions, footAngles, bodyTransform, Time::fixedDeltaTime);

        } else if(turn != 0.0f){

            g_turn_1->StepForward(Time::fixedDeltaTime * turn);

            float t2 = g_turn_1->time + g_turn_1->duration*0.5;
            if(t2 >= g_turn_1->duration){ 
                t2 -= g_turn_1->duration;
            } else if(t2 < 0.0f){
                t2 += g_turn_1->duration;
            }

            g_turn_2->StepTo(t2);
        
            Eigen::Vector3f footPositions[4];
        
            footPositions[0][0] = +g_turn_1->footPositions[0][0];
            footPositions[0][1] = +g_turn_1->footPositions[0][1];
            footPositions[0][2] = +g_turn_1->footPositions[0][2] - dz;

            footPositions[1][0] = -g_turn_2->footPositions[0][1];
            footPositions[1][1] = +g_turn_2->footPositions[0][0];
            footPositions[1][2] = +g_turn_2->footPositions[0][2] - dz;

            footPositions[2][0] = -g_turn_1->footPositions[0][0];
            footPositions[2][1] = -g_turn_1->footPositions[0][1];
            footPositions[2][2] = +g_turn_1->footPositions[0][2] - dz;

            footPositions[3][0] = +g_turn_2->footPositions[0][1];
            footPositions[3][1] = -g_turn_2->footPositions[0][0];
            footPositions[3][2] = +g_turn_2->footPositions[0][2] - dz;

            // LogDebug("WalkerBrain", iLog << footPositions[0]);
            // LogDebug("WalkerBrain", iLog << "t1=" << g_turn_1->time << " / t2=" << g_turn_2->time);
            // LogDebug("WalkerBrain", iLog << footPositions[0][0] << ", " << footPositions[0][1] << ", " << footPositions[0][2]);

            float footAngles[4] = {0.0f, 0.0f, 0.0f, 0.0f};

            Eigen::Affine3f bodyTransform = Eigen::Affine3f::Identity();
            bodyTransform.rotate(Eigen::AngleAxisf(g_walk_1->bodyRotation[1], Eigen::Vector3f::UnitY()));
            bodyTransform.rotate(Eigen::AngleAxisf(g_walk_1->bodyRotation[0], Eigen::Vector3f::UnitX()));
            bodyTransform.rotate(Eigen::AngleAxisf(g_walk_1->bodyRotation[2], Eigen::Vector3f::UnitZ()));

            Eigen::Vector3f bodyPosition = {0.0f, 0.0f, 0.0f};           
            bodyTransform.translate(bodyPosition);
        
            ApplyPoseToJoints(footPositions, footAngles, bodyTransform, Time::fixedDeltaTime);

        } else {
        
            Eigen::Vector3f footPositions[4];
            
            footPositions[0][0] = dx;
            footPositions[0][1] = dy;
            footPositions[0][2] = -0.1f - dz;

            footPositions[1][0] = -dx;
            footPositions[1][1] = +dy;
            footPositions[1][2] = -0.1f - dz;

            footPositions[2][0] = -dx;
            footPositions[2][1] = -dy;
            footPositions[2][2] = -0.1f - dz;

            footPositions[3][0] = +dx;
            footPositions[3][1] = -dy;
            footPositions[3][2] = -0.1f - dz;

            float footAngles[4] = {0.0f, 0.0f, 0.0f, 0.0f};

            Eigen::Affine3f bodyTransform = Eigen::Affine3f::Identity();
            bodyTransform.rotate(Eigen::AngleAxisf(g_walk_1->bodyRotation[1], Eigen::Vector3f::UnitY()));
            bodyTransform.rotate(Eigen::AngleAxisf(g_walk_1->bodyRotation[0], Eigen::Vector3f::UnitX()));
            bodyTransform.rotate(Eigen::AngleAxisf(g_walk_1->bodyRotation[2], Eigen::Vector3f::UnitZ()));

            Eigen::Vector3f bodyPosition = {0.0f, 0.0f, 0.0f};           
            bodyTransform.translate(bodyPosition);
        
            ApplyPoseToJoints(footPositions, footAngles, bodyTransform, Time::fixedDeltaTime);

        }

    }

}

void WalkerBrain::Destroy(){

}

}