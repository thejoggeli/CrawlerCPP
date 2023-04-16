#pragma once

#include <fstream>
#include <iomanip>
#include "core/Log.h"
#include "crawler/Main.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include "math/Mathf.h"

namespace Crawler {

void DebugMotors(Main* main);
void DebugIKFK(Main* main);
void DebugGridSearch(Main* main);

void Debug(Main* main){

    // DebugMotors(main);
    DebugIKFK(main);
    // DebugIKGridSearch(main);

}

void DebugMotors(Main* main){

    LogInfo("Debug", "DebugMotors()");

    Robot* robot = main->robot;

    // reboot servos
    robot->RebootServos(3.5f);

    // move servos to initial position
    for(Joint* joint : robot->jointsList){
        joint->SetTargetAngle(30.0f * DEG_2_RADf);
        joint->MoveServoToTargetAngle(2.5f);
    }

    // read servos angle
    for(Joint* joint : robot->jointsList){
        if(joint->UpdateMeasuredAngle()){
            LogInfo("Debug", iLog << joint->debugName << " angle is " << (joint->measuredAngle*RAD_2_DEGf));
        }
    }

    // read servos current
    float servosCurrentSum = 0.0f;
    for(Joint* joint : robot->jointsList){
        if(joint->UpdateMeasuredCurrent()){
            servosCurrentSum += joint->measuredCurrent;
            LogInfo("Debug", iLog << joint->debugName << " current is " << joint->measuredCurrent);
        }
    }
    LogInfo("Debug", iLog << "total current is " << servosCurrentSum);

}

void DebugIKFK(Main* main){

    LogInfo("Debug", "DebugIKFK()");

    using namespace std;

    Robot* robot = main->robot;
    char buffer[100];

    // FK leg
    Leg* leg = robot->legs[0];

    // FK angles (input)
    LogDebug("Debug", "FK input angles");
    float angles[4] = {
        1.3f * DEG_2_RADf,
        3.7f * DEG_2_RADf,
        4.2f * DEG_2_RADf,
        80.0f * DEG_2_RADf,
    };
    for(int i = 0; i < 4; i++){
        sprintf(buffer, "  a%d: %7.3f deg", i, angles[i] * RAD_2_DEGf);
        LogDebug("Debug", buffer);
    }

    // FK positions (output)
    LogDebug("Debug", "FK output positions");
    leg->FKJoints(angles);
    for(int i = 0; i < 5; i++){
        auto& pos = leg->fkJointsResult.jointPositions[i];
        sprintf(buffer, "  p%d: %6.3f, %6.3f, %6.3f", i, pos[0], pos[1], pos[2]);
        LogDebug("Debug", buffer);
    }

    // FK phi (output)
    LogDebug("Debug", "FK output phi");
    float phi = robot->legs[0]->FKPhi(angles); 
    sprintf(buffer, "  phi: %7.3f deg", phi * RAD_2_DEGf);
    LogDebug("Debug", buffer);


    // IK test
    LogDebug("Debug", "IK output angles (input = FK outut)");
    float anglesIK[4];
    leg->IKExact(leg->fkJointsResult.footPosition, phi, anglesIK);
    for(int i = 0; i < 4; i++){
        sprintf(buffer, "  a%d: %7.3f deg", i, anglesIK[i] * RAD_2_DEGf);
        LogDebug("Debug", buffer);
    }

}

void DebugIKGridSearch(Main* main){

    using namespace std;

    // output file name
    std::string filename = "gridsearch-newleg.csv";

    // grid search leg
    Robot* robot = main->robot;
    Leg* leg = robot->legs[1];

    // xyz range
    float xyzScale = 0.001f;
    int xyzMin = -350;
    int xyzMax = +350;
    int xyzStep = 10;

    // phi range
    int phiMin = -180;
    int phiMax = -180;
    int phiStep = -180;

    // prepate csv file
    ofstream csvFile(string(CRAWLER_ROOT_PATH) + string("/export/") + filename);
    csvFile << "result,a0,a1,a2,a3,x,y,z,phi" << endl;

    // grid search to csv file
    float anglesIK[4];
    for(int x = xyzMin; x <= xyzMax; x+=xyzStep){
        float progress = (float)(x-xyzMin) / 70.0f;
        LogDebug("Debug", iLog << "progress: " << progress*100.0f << "%");
        float xf = (float)x*xyzScale;
        for(int y = xyzMin; y <= xyzMax; y+=xyzStep){
            float yf = (float)y*xyzScale;
            for(int z = xyzMin; z <= xyzMax; z+=xyzStep){
                float zf = (float)z*xyzScale;
                Eigen::Vector3f pos(xf, yf, zf);
                for(int phi = -phiMin; phi <= phiMax; phi+=phiStep){
                    float phif = (float)phi*DEG_2_RADf;
                    bool result = leg->IKExact(pos, phif, anglesIK);
                    stringstream ss;
                    ss << setprecision(6);
                    if(result){
                        ss << "1,";
                        ss << anglesIK[0] << ",";
                        ss << anglesIK[1] << ",";
                        ss << anglesIK[2] << ",";
                        ss << anglesIK[3] << ",";
                    } else {
                        ss << "0,";
                        ss << (int)0 << ",";
                        ss << (int)0 << ",";
                        ss << (int)0 << ",";
                        ss << (int)0 << ",";
                    }
                    ss << xf << "," ;
                    ss << yf << "," ;
                    ss << zf << "," ;
                    ss << phif << "," ;
                    csvFile << ss.str() << endl;
                }                
            }
        }
    }

    // close csv file
    csvFile.close();

}

}