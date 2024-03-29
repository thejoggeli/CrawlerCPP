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

GaitBrain::GaitBrain() : Brain("dance") {

}

GaitBrain::~GaitBrain(){
    if(gait != nullptr){
        delete gait;
    }
}

static float f(float t);
static float g(float t);

static const float a = 0.95f;
static const float b = (1.0f-a)*PIf/2.0f;
static const float k = f(1.0f) - f(0.0f);

static float f(float t){
    return -cos(a*t*PIf+b);
}

static float g(float t){
    return f(t) / k + 0.5f;
}

static float h(float t){
    return (cos((t-1.0f) * PIf)*0.5f+0.5f);
}

void GaitBrain::Init(){

    robot->Startup();
    
    gait = new Gait();
    
    GaitFrame frame;

    for(int i = 0; i < 4; i++){
        Leg* leg = robot->legs[i];
        float angles[4];
        leg->GetJointsCurrentTargetAngles(angles);
        leg->FKJoints(angles);
        frame.footPositions[i] = leg->hipTransform * leg->fkJointsResult.footPosition;
    }

    unsigned int steps = 200;
    float step = 1.0/(float)steps;
    float speed = 0.5f;

    speed = 0.5f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        t = g(t);
        float posx = sin(t*PI2f)*0.07f;
        float posz = cos(t*PI2f)*0.07f + 0.06f;
        frame.bodyPosition = {posx, 0.0f, posz};
        frame.bodyRotation = {0.0f, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    speed = 0.5f;
    for(unsigned int i = 0; i < steps; i++){
        float t = 1.0f - (float)i*step;
        t = g(t);
        float posx = sin(t*PI2f)*0.07f;
        float posz = cos(t*PI2f)*0.07f + 0.06f;
        frame.bodyPosition = {posx, 0.0f, posz};
        frame.bodyRotation = {0.0f, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    speed = 0.5f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        t = g(t);
        float posy = sin(t*PI2f)*0.07f;
        float posz = cos(t*PI2f)*0.07f + 0.06f;
        float rotx = sin(t*PI2f) * 15.0f * DEG_2_RADf;
        frame.bodyPosition = {0.0f, posy, posz};
        frame.bodyRotation = {rotx, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    speed = 0.5f;
    for(unsigned int i = 0; i < steps; i++){
        float t = 1.0f - (float)i*step;
        t = g(t);
        float posy = sin(t*PI2f)*0.07f;
        float posz = cos(t*PI2f)*0.07f + 0.06f;
        float rotx = sin(t*PI2f) * 15.0f * DEG_2_RADf;
        frame.bodyPosition = {0.0f, posy, posz};
        frame.bodyRotation = {rotx, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // move down
    speed = 1.0f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        float posz = 0.13f - h(t) * 0.13f; 
        frame.bodyPosition = {0.0f, 0.0f, posz};
        frame.bodyRotation = {0.0f, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // move forward
    step = 1.0/(float)steps;
    speed = 2.0f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        float posx = 0.0f + h(t) * 0.05f;
        frame.bodyPosition = {posx, 0.0f, 0.0f};
        frame.bodyRotation = {0.0f, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // circle 1
    speed = 0.5f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        t = g(t);
        float posx = cos(t*PI2f) * 0.05f;
        float posy = sin(t*PI2f) * 0.05f;
        frame.bodyPosition = {posx, posy, 0.0f};
        frame.bodyRotation = {0.0f, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // move backward
    speed = 2.0f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        float posx = 0.05f - h(t) * 0.1f;
        frame.bodyPosition = {posx, 0.0f, 0.0f};
        frame.bodyRotation = {0.0f, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // circle 2
    speed = 0.5f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        t = g(t);
        float posx = -cos(t*PI2f) * 0.05f;
        float posy = sin(t*PI2f) * 0.05f;
        frame.bodyPosition = {posx, posy, 0.0f};
        frame.bodyRotation = {0.0f, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // move back to center
    speed = 2.0f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        float posx = - 0.05f + h(t) * 0.05f;
        frame.bodyPosition = {posx, 0.0f, 0.0f};
        frame.bodyRotation = {0.0f, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // rotate prep
    speed = 2.0f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        float posz = h(t) * 0.05f; 
        frame.bodyPosition = {0.0f, 0.0f, posz};
        frame.bodyRotation = {0.0f, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // rotate 1
    speed = 0.25f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        t = g(t);
        float rot = sin(t*PI2f) * 40.0f * DEG_2_RADf;
        float posz = 0.05f;
        frame.bodyPosition = {0.0f, 0.0f, posz};
        frame.bodyRotation = {rot, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }
    

    // rotate 2
    speed = 0.25f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        t = g(t);
        float rot = sin(t*PI2f) * 40.0f * DEG_2_RADf;
        float posz = 0.05f;
        frame.bodyPosition = {0.0f, 0.0f, posz};
        frame.bodyRotation = {0.0f, rot, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // rotate 3
    speed = 0.25f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        t = g(t);
        float rot = sin(t*PI2f) * 20.0f * DEG_2_RADf;
        float posz = 0.05f;
        frame.bodyPosition = {0.0f, 0.0f, posz};
        frame.bodyRotation = {0.0f, 0.0f, rot};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // rotate 4 prep
    speed = 1.0f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        float roty = h(t) * 30.0f * DEG_2_RADf;
        float posz = 0.05f;
        frame.bodyPosition = {0.0f, 0.0f, posz};
        frame.bodyRotation = {0.0f, -roty, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // rotate 4
    speed = 0.5f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        t = g(t);
        float roty = cos(t*PI2f) * 30.0f * DEG_2_RADf;
        float rotx = sin(t*PI2f) * 30.0f * DEG_2_RADf;
        float posz = 0.05f;
        frame.bodyPosition = {0.0f, 0.0f, posz};
        frame.bodyRotation = {rotx, -roty, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // rotate 5
    speed = 0.5;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        t = g(t);
        float roty = cos(t*PI2f) * 30.0f * DEG_2_RADf;
        float rotx = -sin(t*PI2f) * 30.0f * DEG_2_RADf;
        float posz = 0.05f;
        frame.bodyPosition = {0.0f, 0.0f, posz};
        frame.bodyRotation = {rotx, -roty, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // rotate 4+5 prep undo
    speed = 1.0f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        float roty = 30.0f * DEG_2_RADf - h(t) * 30.0f * DEG_2_RADf;
        float posz = 0.05f - h(t) * 0.05f; 
        frame.bodyPosition = {0.0f, 0.0f, posz};
        frame.bodyRotation = {0.0f, -roty, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }
    
    // // rotate-shift prep
    // speed = 1.0f;
    // float shift_r = 0.05;
    // for(unsigned int i = 0; i < steps; i++){
    //     float t = (float)i*step;
    //     float roty = 30.0f * DEG_2_RADf - h(t) * 10.0f * DEG_2_RADf;
    //     float posx = h(t) * shift_r;
    //     float posz = 0.05f;
    //     frame.bodyPosition = {posx, 0.0f, posz};
    //     frame.bodyRotation = {0.0f, -roty, 0.0f};
    //     gait->AddFrame(frame);
    //     gait->AddTransition(step/speed);
    // }

    // rotate-shift 1
    // speed = 0.25;
    // for(unsigned int i = 0; i < steps; i++){
    //     float t = (float)i*step;
    //     t = g(t);
    //     float roty = cos(t*PI2f) * 20.0f * DEG_2_RADf;
    //     float rotx = sin(t*PI2f) * 20.0f * DEG_2_RADf;
    //     float posz = 0.05f;
    //     float posx = cos(t*PI2f) * shift_r;
    //     float posy = sin(t*PI2f) * shift_r;
    //     frame.bodyPosition = {posx, posy, posz};
    //     frame.bodyRotation = {rotx, -roty, 0.0f};
    //     gait->AddFrame(frame);
    //     gait->AddTransition(step/speed);
    // }

    // rotate-shift 2
    // speed = 0.25;
    // for(unsigned int i = 0; i < steps; i++){
    //     float t = (float)i*step;
    //     t = g(t);
    //     float roty = cos(t*PI2f) * 20.0f * DEG_2_RADf;
    //     float rotx = -sin(t*PI2f) * 20.0f * DEG_2_RADf;
    //     float posz = 0.05f;
    //     float posx = cos(t*PI2f) * shift_r;
    //     float posy = -sin(t*PI2f) * shift_r;
    //     frame.bodyPosition = {posx, posy, posz};
    //     frame.bodyRotation = {rotx, -roty, 0.0f};
    //     gait->AddFrame(frame);
    //     gait->AddTransition(step/speed);
    // }

    // rotate-shift undo
    // speed = 1.0f;
    // for(unsigned int i = 0; i < steps; i++){
    //     float t = (float)i*step;
    //     float roty = 20.0f * DEG_2_RADf - h(t) * 20.0f * DEG_2_RADf;
    //     float posz = 0.05f - h(t) * 0.05f; 
    //     float posx = shift_r - h(t) * shift_r; 
    //     frame.bodyPosition = {posx, 0.0f, posz};
    //     frame.bodyRotation = {0.0f, -roty, 0.0f};
    //     gait->AddFrame(frame);
    //     gait->AddTransition(step/speed);
    // }

    // move up
    speed = 1.0f;
    for(unsigned int i = 0; i < steps; i++){
        float t = (float)i*step;
        float posz = h(t) * 0.13f; 
        frame.bodyPosition = {0.0f, 0.0f, posz};
        frame.bodyRotation = {0.0f, 0.0f, 0.0f};
        gait->AddFrame(frame);
        gait->AddTransition(step/speed);
    }

    // float dxy = 0.05f;

    // frame.bodyPosition = {dxy, 0.0f, 0.0f};
    // gait->AddFrame(frame);
    // gait->AddTransition(1.0f);

    // frame.bodyPosition = {0.0f, dxy, 0.0f};
    // gait->AddFrame(frame);
    // gait->AddTransition(1.0f);

    // frame.bodyPosition = {dxy, 0.0f, 0.0f};
    // gait->AddFrame(frame);
    // gait->AddTransition(1.0f);

    // frame.bodyPosition = {0.0f, -dxy, 0.0f};
    // gait->AddFrame(frame);
    // gait->AddTransition(1.0);
    
    // frame.bodyPosition = {0.0f, 0.0f, -0.03f};
    // gait->AddFrame(frame);
    // gait->AddTransition(1.0f);

    // frame.bodyPosition = {0.0f, 0.0f, 0.13f};
    // gait->AddFrame(frame);
    // gait->AddTransition(1.0f);

    // float dxy = 0.05f;

    // frame.bodyPosition = {0.0f, dxy, 0.04f};
    // frame.bodyRotation = {0.0f, 0.0f, -20.0f * DEG_2_RADf};
    // gait->AddFrame(frame);
    // gait->AddTransition(1.0f);

    // frame.bodyPosition = {0.0f, 0.0f, -0.01f};
    // frame.bodyRotation = {20.0f * DEG_2_RADf, 0.0f, 0.0f};
    // gait->AddFrame(frame);
    // gait->AddTransition(1.0f);

    // frame.bodyPosition = {0.0f, -dxy, 0.04f}; 
    // frame.bodyRotation = {0.0f, 0.0f, 20.0f * DEG_2_RADf};
    // gait->AddFrame(frame);
    // gait->AddTransition(1.0f);

    // frame.bodyPosition = {0.0f, 0.0f, -0.01f};
    // frame.bodyRotation = {20.0f * DEG_2_RADf, 0.0f, 0.0f};
    // gait->AddFrame(frame);
    // gait->AddTransition(1.0f);

    // for(int i = 0; i < 4; i++){

    //     float angle = (float)i*PI2f*0.25f + PI2f*0.125f;
    //     Leg* leg = robot->legs[i];
        
    //     float body_xy = -0.06f;
    //     float body_z = 0.0f;
    //     float foot_xy = 0.0f;
    //     float foot_z = 0.1f;
    //     Eigen::Vector3f footDown = frame.footPositions[i];
    //     Eigen::Vector3f footUp = footDown + Eigen::Vector3f(cos(angle)*foot_xy, sin(angle)*foot_xy, foot_z);
    //     // Eigen::Vector3f footUp = leg->hipTransform * (leg->hipTransformInverse * footDown + Eigen::Vector3f(foot_xy, 0.0f, foot_z));

    //     frame.bodyPosition = Eigen::Vector3f(cos(angle)*body_xy, sin(angle)*body_xy, body_z);
    //     gait->AddFrame(frame); 
    //     gait->AddTransition(1.0);

    //     frame.footPositions[i] = footUp;
    //     gait->AddFrame(frame);
    //     gait->AddTransition(1.0f);

    //     frame.footPositions[i] = footDown;
    //     gait->AddFrame(frame); 
    //     gait->AddTransition(1.0);

        // frame.bodyPosition = Eigen::Vector3f(0, 0, 0.06f);
        // gait->AddFrame(frame);
        // gait->AddTransition(1.0);

        // frame.bodyRotation = Eigen::Vector3f(20.0f * DEG_2_RADf, 0.0f, 0.0f);
        // // frame.footPositions[i] = footUp;
        // gait->AddFrame(frame);
        // gait->AddTransition(1.0);

        // frame.bodyRotation = Eigen::Vector3f(-20.0f * DEG_2_RADf, 0.0f, 0.0f);
        // // frame.footPositions[i] = footDown;
        // gait->AddFrame(frame);
        // gait->AddTransition(1.0);

    //     frame.bodyRotation = Eigen::Vector3f(0, 0, 0);
    //     gait->AddFrame(frame);
    //     gait->AddTransition(1.0);

    // }

}

void GaitBrain::Update(){

    auto& clients = ClientManager::GetAllCients();
    if(clients.size() > 0){
        Client* client = clients[0].get();

        if(client->OnKeyDown(GamepadKey::A)){
            playing = !playing;
            LogDebug("GaitBrain", iLog << "playing=" << (int)playing);
        }

    }

}

void GaitBrain::FixedUpdate(){

    auto& clients = ClientManager::GetAllCients();
    if(clients.size() > 0){
        Client* client = clients[0].get();

        if(client->IsKeyDown(GamepadKey::Up)){
            speed += Time::fixedDeltaTime;
            LogDebug("GaitBrain", iLog << "speed=" << speed);
        }
        if(client->IsKeyDown(GamepadKey::Down)){
            speed -= Time::fixedDeltaTime;
            LogDebug("GaitBrain", iLog << "speed=" << speed);
        }
    }
    
    if(playing){
        gait->StepForward(Time::fixedDeltaTime * speed);

        // const Eigen::Vector3f& p = gait->bodyPosition;
        // LogInfo("GaitBrain", iLog << "bodyPosition=(" << p.x() << ", " << p.y() << ", " << p.z() << ")" );

        // const Eigen::Vector3f& p = gait->footPositions[0];
        // LogInfo("GaitBrain", iLog << "footPosition[0]=(" << p.x() << ", " << p.y() << ", " << p.z() << ")" );

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