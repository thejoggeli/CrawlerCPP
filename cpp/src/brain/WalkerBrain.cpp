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

WalkerBrain::WalkerBrain() : Brain() {

}

WalkerBrain::~WalkerBrain(){
    if(g_walk_1 != nullptr){
        delete g_walk_1;
    }
    if(g_walk_2 != nullptr){
        delete g_walk_2;
    }
}

void WalkerBrain::Init(){
    
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

    float t0 = 0.5f;
    float t1 = 0.04f;
    float t2 = 0.04f;
    float t3 = 0.04f;

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

void WalkerBrain::Update(){

    auto& clients = ClientManager::GetAllCients();
    if(clients.size() > 0){
        Client* client = clients[0].get();

        if(client->OnKeyDown(GamepadKey::A)){
            playing = !playing;
            LogDebug("WalkerBrain", iLog << "playing=" << (int)playing);
        }

    }

}

void WalkerBrain::FixedUpdate(){

    auto& clients = ClientManager::GetAllCients();
    if(clients.size() > 0){
        Client* client = clients[0].get();

        if(client->IsKeyDown(GamepadKey::Up)){
            speed += Time::fixedDeltaTime;
            LogDebug("WalkerBrain", iLog << "speed=" << speed);
        }
        if(client->IsKeyDown(GamepadKey::Down)){
            speed -= Time::fixedDeltaTime;
            LogDebug("WalkerBrain", iLog << "speed=" << speed);
        }
    }
    
    if(playing){


        auto& clients = ClientManager::GetAllCients();
        Client* client = nullptr;

        if(clients.size() > 0){
            client = clients[0].get();
        }

        if(client){
        
            Eigen::Vector2f input = client->GetJoystickPosition(GamepadKey::LeftJoystick);

            float jx = -input[1];
            float jy = -input[0];
            float jlen = sqrtf(jx*jx+jy*jy);

            if(jlen > 0.01f){
                float jangle = atan2f(jy,jx);

                g_walk_1->StepForward(Time::fixedDeltaTime * jlen);

                float t2 = g_walk_1->time + g_walk_1->duration*0.5;
                if(t2 >= g_walk_1->duration){ 
                    t2 -= g_walk_1->duration;
                }

                g_walk_2->StepTo(t2);

                LogDebug("WalkerBrain", iLog << g_walk_1->currFrameIndex << "-" << g_walk_1->nextFrameIndex << " // " << g_walk_2->currFrameIndex << "-" << g_walk_2->nextFrameIndex);
                    
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

                float dx = 0.16f;
                float dy = 0.16f;

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

                footPositions[1][0] -= dx;
                footPositions[1][1] += dy;

                footPositions[2][0] -= dx;
                footPositions[2][1] -= dy;

                footPositions[3][0] += dx;
                footPositions[3][1] -= dy;
                
                ApplyPoseToJoints(footPositions, footAngles, bodyTransform, Time::fixedDeltaTime);

            }

        }

    }

}

void WalkerBrain::Destroy(){

}

}