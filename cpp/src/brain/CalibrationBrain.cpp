
#include "CalibrationBrain.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include "core/Log.h"
#include "remote/ClientManager.h"
#include "remote/Packet.h"
#include "core/Config.h"
#include "math/Mathf.h"

using namespace std;

namespace Crawler {


CalibrationBrain::CalibrationBrain() : Brain("calib") {
    
}

void CalibrationBrain::Init(){

    robot->TorqueOff();

    ClientManager::SubscribeMessage("setCalib", this, [](void* caller, PacketMessage& msg){
        
        CalibrationBrain* self = (CalibrationBrain*)caller;
        Robot* robot = self->robot;

        // get joint
        int jointId = msg.GetInt("jointId");
        if(jointId < 0 || jointId >= robot->jointsList.size()){
            LogError("CalibrationBrain", iLog << "setCalib invalid joint id: " << jointId);
            return;
        }
        Joint* joint = robot->jointsList[jointId];

        // get calibration value number
        int valueNumber = msg.GetInt("valueNumber");
        if(valueNumber < 0 || valueNumber >= 3){
            LogError("CalibrationBrain", iLog << "setCalib invalid value number: " << valueNumber);
            return;
        }

        // set new calibration value
        unsigned int newValue = joint->measuredXYZ;
        joint->SetCalibrationValue(newValue, valueNumber);
        joint->UpdateCalibrationFactors();

        // save to config file
        char buffer[20];
        sprintf(buffer, "%.0fdeg", joint->GetCalibrationAngle(valueNumber) * RAD_2_DEGf);
        LogInfo("CalibrationBrain", iLog 
            << "calibration value of joint " << joint->nameWithLeg 
            << " at angle " << buffer
            << " set to " << newValue
        );
        if(valueNumber == 0){
            Config::Set("calib", "low_" + std::to_string(jointId), (int) newValue);
        } else if(valueNumber == 1){
            Config::Set("calib", "mid_" + std::to_string(jointId), (int) newValue);
        } else if(valueNumber == 1){
            Config::Set("calib", "high_" + std::to_string(jointId), (int) newValue);
        }

        Config::WriteFile("calib");

        // response
        auto response = std::make_shared<PacketMessage>("setCalibResponse");
        response->AddInt("jointId", jointId); // joint id
        response->AddInt("valueNumber", valueNumber);
        response->AddInt("value", newValue);
        ClientManager::SendPacket(response, msg.clientId);

    });
}

void CalibrationBrain::Update(){
    

}

void CalibrationBrain::FixedUpdate(){

}

void CalibrationBrain::Destroy(){
    ClientManager::UnsubscribeMessage("setCalib", this);
}

}