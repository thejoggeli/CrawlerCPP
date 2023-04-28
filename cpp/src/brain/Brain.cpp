#include "Brain.h"

#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include "core/Log.h"
#include "core/Time.h"
#include <cassert>

using namespace std;

namespace Crawler {

Brain::Brain() {

}

void Brain::SetRobot(Robot* robot){
    this->robot = robot;
}

bool Brain::ApplyPoseToJoints(Eigen::Vector3f footPositions[4], float footAngles[4], const Eigen::Affine3f& bodyTransform, float playtime){

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

        Eigen::Vector3f footPositionTransformed = footPositions[i];
        footPositionTransformed = bodyTransform.inverse() * footPositionTransformed;
        footPositionTransformed = leg->hipTransformInverse * footPositionTransformed;

        bool result = leg->IKSearch(footPositionTransformed, footAngles[i], oldPhi, oldAngles, outAngles[i]);

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
    } else {
        // IK failed
        LogDebug("Brain", "inverse kinematics search failed");
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

}