#include "Robot.h"
#include "Leg.h"
#include "Joint.h"
#include "brain/Brain.h"
#include "parts/XYZServo.h"
#include "comm/SerialStream.h"
#include "math/Mathf.h"
#include "core/Log.h"
#include "core/Time.h"

using namespace std;

namespace Crawler {

Robot::Robot(){ 

    // create serial stream for servo communication
    servoSerialStream = new SerialStream();

    // all servo ids in order [hip, knee, knee, knee] 
    servoIds = {
         7,  8,  9, 10, // front-left  
         1,  2,  4,  3, // back-left   
        15, 16, 17, 18, // back-right  
        11, 12, 13, 14, // front right 
    };

    // servo angle scale
    float servoAngleScale[] = {
        -1.05f, -1.05f, -1.05f, -1.05f, // front-left  
        -1.05f, +1.05f, +1.05f, +1.05f, // back-left   
        -1.05f, -1.05f, -1.05f, -1.05f, // back-right  
        -1.05f, -1.05f, -1.05f, -1.05f, // front right 
    };

    // create master servo
    masterServo = new XYZServo(servoSerialStream, 254);

    // create joint servos
    for(int i = 0; i < servoIds.size(); i++){
        XYZServo* servo = new XYZServo(servoSerialStream, servoIds[i]);
        jointServos.push_back(servo);
    }

    // create the legs
    for(int i = 0; i < 4; i++){

        Leg* leg = new Leg();
        legs.push_back(leg);

        // set leg joint lengths
        leg->joints[0]->length = 0.068;
        leg->joints[1]->length = 0.068;
        leg->joints[2]->length = 0.068;
        leg->joints[3]->length = 0.096;

        // add all joints of current leg to jointsList
        for(Joint* joint : leg->joints){
            jointsList.push_back(joint);
        }

    }

    // assign servo motors to all joints
    for(int i = 0; i < jointsList.size(); i++){
        jointsList[i]->SetServo(jointServos[i]);
        jointsList[i]->SetServoAngleScale(servoAngleScale[i]);
    }

    // set legs hip transform
    float hip_diagonal = 0.155*0.5;
    float hip_dx = hip_diagonal * SQRT2_INVf;
    float hip_dy = hip_diagonal * SQRT2_INVf;
    float hip_dz = 0.0f;

    legs[0]->SetHipTransform(Eigen::Vector3f(+hip_dx, +hip_dy, hip_dz), 45.0f * DEG_2_RADf);
    legs[2]->SetHipTransform(Eigen::Vector3f(-hip_dx, -hip_dy, hip_dz), -135.0f * DEG_2_RADf);
    legs[3]->SetHipTransform(Eigen::Vector3f(+hip_dx, -hip_dy, hip_dz), -45.0f * DEG_2_RADf);

    // set new leg hip transform and lengths
    hip_diagonal = 0.068;
    hip_dx = hip_diagonal * SQRT2_INVf;
    hip_dy = hip_diagonal * SQRT2_INVf;
    hip_dz = 0.0f;
    legs[1]->SetHipTransform(Eigen::Vector3f(-hip_dx, +hip_dy, hip_dz), 135.0f * DEG_2_RADf);
    legs[1]->joints[0]->length = 0.068;
    legs[1]->joints[1]->length = 0.078;
    legs[1]->joints[2]->length = 0.078;
    legs[1]->joints[3]->length = 0.027 + 0.063 + 0.01;
    legs[1]->joints[1]->limitMin = -100.0f * DEG_2_RADf;
    legs[1]->joints[1]->limitMax = +100.0f * DEG_2_RADf;
    legs[1]->joints[2]->limitMin = -150.0f * DEG_2_RADf;
    legs[1]->joints[2]->limitMax = +150.0f * DEG_2_RADf;

    // create position buffer
    for(int i = 0; i < servoIds.size(); i++){
        servoGoalBuffer.push_back(0);
    }

}

Robot::~Robot(){ 
    for(Leg* leg : legs){
        delete leg;
    }
    legs.clear();
    delete masterServo;
}

void Robot::SetBrain(Brain* brain){

    if(this->brain != nullptr){
        this->brain->Destroy();
    }

    this->brain = brain;
    this->brain->SetRobot(this);

    brain->Init();

}

bool Robot::OpenSerialStream(const char* device){
    int ret = servoSerialStream->open(device, 115200);
    return ret;
}

void Robot::CloseSerialStream(){
    servoSerialStream->close();
}

void Robot::MoveJointsToTargetSync(float time){
    for(int i = 0; i < jointsList.size(); i++){
        Joint* joint = jointsList[i];
        servoGoalBuffer[i] = joint->AngleToXYZ(joint->currentTargetAngle);
    }
    uint8_t playtime = (uint8_t)(time*100.0f);
    masterServo->setPositionsSync(servoGoalBuffer.data(), servoIds.data(), playtime, servoIds.size());
}

void Robot::Update(){
    brain->Update();
}

void Robot::FixedUpdate(){
    brain->FixedUpdate();
}

void Robot::RebootServos(float sleepTime = 3.5f){
    LogInfo("Robot", iLog << "rebooting servos (sleepTime=" << sleepTime << ")");
    for(XYZServo* servo : jointServos){
        servo->reboot();
    }
    Time::Sleep(sleepTime);
}

void Robot::Startup(){

    LogInfo("Robot", "startup");

    // reboot servos
    RebootServos(3.5f);

    // move servos to initial position
    LogInfo("Robot", "moving to default position");
    for(Leg* leg : legs){
        leg->joints[0]->SetTargetAngle(DEG_2_RADf * 0.0f);
        leg->joints[1]->SetTargetAngle(DEG_2_RADf * -30.0f);
        leg->joints[2]->SetTargetAngle(DEG_2_RADf * -30.0f);
        leg->joints[3]->SetTargetAngle(DEG_2_RADf * -30.0f);
    }
    MoveJointsToTargetSync(2.0f);
    Time::Sleep(2.5f);
    
}

void Robot::Shutdown(){

    LogInfo("Robot", "shutdown");

}

}