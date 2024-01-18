
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

ModelBrain::ModelBrain() : Brain() {

}

ModelBrain::~ModelBrain() {
    if(onnxRunner){
        delete onnxRunner;
    }
}

void ModelBrain::Init(){

    onnxRunner = new OnnxRunner("Crawler2.onnx");

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

    // set input values [0, 15] -> current angles  
    const float dof_pos_scale = 1.0f;
    for(int i = 0; i < measuredAngles.size(); i++){
        inputValues[i] = measuredAngles[i] * dof_pos_scale;
    }

    // set input values [16-31] -> current angle velocities
    const float dof_vel_scale = 0.05f;
    for(int i = 0; i < angleVelocities.size(); i++){
        inputValues[i+16] = angleVelocities[i] * dof_vel_scale;
    }

    // set input values [32-34] -> imu acceleration values
    inputValues[32] = 0.0f; // TODO
    inputValues[33] = 0.0f; // TODO
    inputValues[34] = 0.0f; // TODO

    // set input values [35-37] -> imu gyro values
    inputValues[35] = 0.0f; // TODO
    inputValues[36] = 0.0f; // TODO
    inputValues[37] = 0.0f; // TODO

    // set input values [38-39] -> command (x,y) values
    inputValues[38] = 1.0f; // TODO
    inputValues[39] = 0.0f; // TODO

    // set input values [40-55] -> previous output values
    for(int i = 0; i < outputValues.size(); i++){
        inputValues[i+40] = outputValues[i];
    }    

    // run inference
    onnxRunner->SetInputValues(inputValues);
    onnxRunner->Run();
    onnxRunner->GetOutputValues(outputValues);

    // apply output angles
    ApplyTargetAngles();

    // remember old angles
    for(int i = 0; i < measuredAngles.size(); i++){
        oldMeasuredAngles[i] = measuredAngles[i];
    }

}

float ModelBrain::OutputToAngle(float val, float low, float high){
    return (val * 0.5f + 0.5f) * (high-low) + low;
}

void ModelBrain::ApplyTargetAngles(){

    float angles[16];

    angles[0+0]  = OutputToAngle(outputValues[0+0],  -45.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[0+4]  = OutputToAngle(outputValues[0+4],  -45.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[0+8]  = OutputToAngle(outputValues[0+8],  -45.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[0+12] = OutputToAngle(outputValues[0+12], -45.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);

    angles[1+0]  = OutputToAngle(outputValues[1+0],  -90.0*DEG_2_RADf, 90.0f*DEG_2_RADf);
    angles[1+4]  = OutputToAngle(outputValues[1+4],  -90.0*DEG_2_RADf, 90.0f*DEG_2_RADf);
    angles[1+8]  = OutputToAngle(outputValues[1+8],  -90.0*DEG_2_RADf, 90.0f*DEG_2_RADf);
    angles[1+12] = OutputToAngle(outputValues[1+12], -90.0*DEG_2_RADf, 90.0f*DEG_2_RADf);

    angles[2+0]  = OutputToAngle(outputValues[2+0],  -150.0f*DEG_2_RADf, 150.0f*DEG_2_RADf);
    angles[2+4]  = OutputToAngle(outputValues[2+4],  -150.0f*DEG_2_RADf, 150.0f*DEG_2_RADf);
    angles[2+8]  = OutputToAngle(outputValues[2+8],  -150.0f*DEG_2_RADf, 150.0f*DEG_2_RADf);
    angles[2+12] = OutputToAngle(outputValues[2+12], -150.0f*DEG_2_RADf, 150.0f*DEG_2_RADf);

    angles[2+0]  = OutputToAngle(outputValues[3+0],  -15.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[2+4]  = OutputToAngle(outputValues[3+4],  -15.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[2+8]  = OutputToAngle(outputValues[3+8],  -15.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
    angles[2+12] = OutputToAngle(outputValues[3+12], -15.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);


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