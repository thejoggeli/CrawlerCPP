
#include "EmptyBrain.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include "core/Log.h"

using namespace std;

namespace Crawler {


EmptyBrain::EmptyBrain() : Brain("empty") {
    
}

void EmptyBrain::Init(){
    // LogDebug("EmptyBrain", "torque off");
    // for(Joint* joint : robot->jointsList){
    //     joint->TorqueOff();
    // }
}

void EmptyBrain::Update(){

}

void EmptyBrain::FixedUpdate(){

}

void EmptyBrain::Destroy(){

}

}