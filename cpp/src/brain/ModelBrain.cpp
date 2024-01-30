
#include "ModelBrain.h"
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
#include "util/OnnxRunner.h"

using namespace std;

namespace Crawler {

ModelBrain::ModelBrain() : Brain("model") {

}

ModelBrain::~ModelBrain() {
    if(onnxRunner){
        delete onnxRunner;
    }
}

void ModelBrain::Init(){

    robot->Startup();

    onnxRunner = new OnnxRunner("Crawler2.pth.onnx");

    // initialize input values vector
    inputValues = std::vector<float> (onnxRunner->GetInputSize());    
    LogDebug("ModelBrain", iLog << "init input values n=" << inputValues.size());
    for(int i = 0; i < inputValues.size(); i++){
        inputValues[i] = 0;
    }

    // initialize output values vector
    outputValues = std::vector<float> (onnxRunner->GetOutputSize());
    LogDebug("ModelBrain", iLog << "init output values n=" << outputValues.size());
    for(int i = 0; i < outputValues.size(); i++){
        outputValues[i] = 0;
    }

    // initialize measured angles
    measuredAngles = std::vector<float>(16);
    LogDebug("ModelBrain", iLog << "init measured angles n=" << measuredAngles.size());
    UpdateMeasuredAngles();

    // initialize old measured angles
    oldMeasuredAngles = std::vector<float>(16);
    LogDebug("ModelBrain", iLog << "init old measured angles n=" << oldMeasuredAngles.size());
    for(int i = 0; i < measuredAngles.size(); i++){
        oldMeasuredAngles[i] = measuredAngles[i];
    }

    // initialize angle velocities
    angleVelocities = std::vector<float>(16);
    LogDebug("ModelBrain", iLog << "init angle velocities n=" << angleVelocities.size());
    for(int i = 0; i < measuredAngles.size(); i++){
        angleVelocities[i] = 0;
    }

    // initialize myAcc
    myAcc = std::vector<float>(16);
    for(int i = 0; i < myAcc.size(); i++){
        myAcc[i] = 0;
    }

    // initialize myVel
    myVel = std::vector<float>(16);
    for(int i = 0; i < myVel.size(); i++){
        myVel[i] = 0;
    }

    // initialize myPos
    myPos = std::vector<float>(16);
    for(int i = 0; i < myPos.size(); i++){
        myPos[i] = measuredAngles[i];
    }

}

void ModelBrain::UpdateMeasuredAngles(){
    measuredAngles[0]  = robot->legs[0]->joints[0]->measuredAngle;
    measuredAngles[1]  = robot->legs[0]->joints[1]->measuredAngle;
    measuredAngles[2]  = robot->legs[0]->joints[2]->measuredAngle;
    measuredAngles[3]  = robot->legs[0]->joints[3]->measuredAngle;
    measuredAngles[4]  = robot->legs[1]->joints[0]->measuredAngle;
    measuredAngles[5]  = robot->legs[1]->joints[1]->measuredAngle;
    measuredAngles[6]  = robot->legs[1]->joints[2]->measuredAngle;
    measuredAngles[7]  = robot->legs[1]->joints[3]->measuredAngle;
    measuredAngles[8]  = robot->legs[2]->joints[0]->measuredAngle;
    measuredAngles[9]  = robot->legs[2]->joints[1]->measuredAngle;
    measuredAngles[10] = robot->legs[2]->joints[2]->measuredAngle;
    measuredAngles[11] = robot->legs[2]->joints[3]->measuredAngle;
    measuredAngles[12] = robot->legs[3]->joints[0]->measuredAngle;
    measuredAngles[13] = robot->legs[3]->joints[1]->measuredAngle;
    measuredAngles[14] = robot->legs[3]->joints[2]->measuredAngle;
    measuredAngles[15] = robot->legs[3]->joints[3]->measuredAngle;
}

void ModelBrain::Update(){

}

void ModelBrain::FixedUpdate(){

    // get current measured angles
    UpdateMeasuredAngles();

    // compute current angle velocities
    for(int i = 0; i < measuredAngles.size(); i++){
        angleVelocities[i] = (measuredAngles[i] - oldMeasuredAngles[i]) / Time::fixedDeltaTime;
    }

    // begin input values
    int idx = 0;

    // set input values - current angles  
    const float dof_pos_scale = 1.0f;
    for(int i = 0; i < measuredAngles.size(); i++){
        inputValues[idx++] = measuredAngles[i] * dof_pos_scale;
    }

    // set input values - current angle velocities
    const float dof_vel_scale = 0.2f;
    for(int i = 0; i < angleVelocities.size(); i++){
        inputValues[idx++] = angleVelocities[i] * dof_vel_scale;
    }

    // set input values - myVel
    for(int i = 0; i < myVel.size(); i++){
        inputValues[idx++] = myVel[i];
    }

    // set input values - myPos
    for(int i = 0; i < myPos.size(); i++){
        inputValues[idx++] = myPos[i];
    }

    // set input values - actions
    for(int i = 0; i < outputValues.size(); i++){
        inputValues[idx++] = outputValues[i];
    }

    // // set input values - imu acceleration values
    // float x = -robot->imu_acc[1];
    // float y = +robot->imu_acc[0];
    // float z = +robot->imu_acc[2];
    // float mag = sqrtf(x*x + y*y + z*z);
    // float magInv = 1.0f/mag;
    // x *= magInv;
    // y *= magInv;
    // z *= magInv;
    // inputValues[idx++] = x;
    // inputValues[idx++] = y;
    // inputValues[idx++] = z;

    // // set input values - command (x,y) values
    // inputValues[idx++] = 1.0f; // TODO
    // inputValues[idx++] = 0.0f; // TODO

    // run inference
    onnxRunner->SetInputValues(inputValues);
    onnxRunner->Run();
    onnxRunner->GetOutputValues(outputValues);

    // char buffer[1000];
    // sprintf(buffer, "%8.4f %8.4f %8.4f %8.4f ... %8.4f %8.4f %8.4f %8.4f", outputValues[0], outputValues[1], outputValues[2], outputValues[3], outputValues[12], outputValues[13], outputValues[14], outputValues[15]);
    // LogDebug("ModelBrain", iLog << buffer);

    // apply output angles
    // ApplyTargetAngles();
    ApplyTargetAccelerations();

    // remember old angles
    for(int i = 0; i < measuredAngles.size(); i++){
        oldMeasuredAngles[i] = measuredAngles[i];
    }

}

float ModelBrain::OutputFromTo(float val, float low, float high){
    return (val * 0.5f + 0.5f) * (high-low) + low;
}

void ModelBrain::ApplyTargetAccelerations(){

    for(int i = 0; i < 16; i++){
        myAcc[i] = OutputFromTo(outputValues[i], -25, 25);
        myVel[i] += myAcc[i] * Time::fixedDeltaTime;
        myPos[i] += myVel[i] * Time::fixedDeltaTime;
    }

    robot->legs[0]->joints[0]->SetTargetAngle(myPos[0],  true);
    robot->legs[0]->joints[1]->SetTargetAngle(myPos[1],  true);
    robot->legs[0]->joints[2]->SetTargetAngle(myPos[2],  true);
    robot->legs[0]->joints[3]->SetTargetAngle(myPos[3],  true);
    robot->legs[1]->joints[0]->SetTargetAngle(myPos[4],  true);
    robot->legs[1]->joints[1]->SetTargetAngle(myPos[5],  true);
    robot->legs[1]->joints[2]->SetTargetAngle(myPos[6],  true);
    robot->legs[1]->joints[3]->SetTargetAngle(myPos[7],  true);
    robot->legs[2]->joints[0]->SetTargetAngle(myPos[8],  true);
    robot->legs[2]->joints[1]->SetTargetAngle(myPos[9],  true);
    robot->legs[2]->joints[2]->SetTargetAngle(myPos[10], true);
    robot->legs[2]->joints[3]->SetTargetAngle(myPos[11], true);
    robot->legs[3]->joints[0]->SetTargetAngle(myPos[12], true);
    robot->legs[3]->joints[1]->SetTargetAngle(myPos[13], true);
    robot->legs[3]->joints[2]->SetTargetAngle(myPos[14], true);
    robot->legs[3]->joints[3]->SetTargetAngle(myPos[15], true);

}

void ModelBrain::ApplyTargetAngles(){

    float angles[16];

    angles[0+0]  = OutputFromTo(outputValues[0+0],  -45.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[0+4]  = OutputFromTo(outputValues[0+4],  -45.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[0+8]  = OutputFromTo(outputValues[0+8],  -45.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[0+12] = OutputFromTo(outputValues[0+12], -45.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);

    angles[1+0]  = OutputFromTo(outputValues[1+0],  -90.0*DEG_2_RADf, 90.0f*DEG_2_RADf);
    angles[1+4]  = OutputFromTo(outputValues[1+4],  -90.0*DEG_2_RADf, 90.0f*DEG_2_RADf);
    angles[1+8]  = OutputFromTo(outputValues[1+8],  -90.0*DEG_2_RADf, 90.0f*DEG_2_RADf);
    angles[1+12] = OutputFromTo(outputValues[1+12], -90.0*DEG_2_RADf, 90.0f*DEG_2_RADf);

    angles[2+0]  = OutputFromTo(outputValues[2+0],  -90.0f*DEG_2_RADf, 150.0f*DEG_2_RADf);
    angles[2+4]  = OutputFromTo(outputValues[2+4],  -90.0f*DEG_2_RADf, 150.0f*DEG_2_RADf);
    angles[2+8]  = OutputFromTo(outputValues[2+8],  -90.0f*DEG_2_RADf, 150.0f*DEG_2_RADf);
    angles[2+12] = OutputFromTo(outputValues[2+12], -90.0f*DEG_2_RADf, 150.0f*DEG_2_RADf);

    angles[2+0]  = OutputFromTo(outputValues[3+0],  -15.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[2+4]  = OutputFromTo(outputValues[3+4],  -15.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[2+8]  = OutputFromTo(outputValues[3+8],  -15.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[2+12] = OutputFromTo(outputValues[3+12], -15.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);

    robot->legs[0]->joints[0]->SetTargetAngle(angles[0],  true);
    robot->legs[0]->joints[1]->SetTargetAngle(angles[1],  true);
    robot->legs[0]->joints[2]->SetTargetAngle(angles[2],  true);
    robot->legs[0]->joints[3]->SetTargetAngle(angles[3],  true);
    robot->legs[1]->joints[0]->SetTargetAngle(angles[4],  true);
    robot->legs[1]->joints[1]->SetTargetAngle(angles[5],  true);
    robot->legs[1]->joints[2]->SetTargetAngle(angles[6],  true);
    robot->legs[1]->joints[3]->SetTargetAngle(angles[7],  true);
    robot->legs[2]->joints[0]->SetTargetAngle(angles[8],  true);
    robot->legs[2]->joints[1]->SetTargetAngle(angles[9],  true);
    robot->legs[2]->joints[2]->SetTargetAngle(angles[10], true);
    robot->legs[2]->joints[3]->SetTargetAngle(angles[11], true);
    robot->legs[3]->joints[0]->SetTargetAngle(angles[12], true);
    robot->legs[3]->joints[1]->SetTargetAngle(angles[13], true);
    robot->legs[3]->joints[2]->SetTargetAngle(angles[14], true);
    robot->legs[3]->joints[3]->SetTargetAngle(angles[15], true);
}

void ModelBrain::Destroy(){

}

}