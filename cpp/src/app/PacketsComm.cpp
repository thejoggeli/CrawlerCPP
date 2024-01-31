#include "PacketsComm.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include "App.h"
#include "remote/ClientManager.h"
#include "remote/Packet.h"
#include "core/Log.h"
#include "math/Mathf.h"
#include "brain/CalibrationBrain.h"
#include "brain/WalkerBrain.h"
#include "brain/SurferBrain.h"
#include "brain/GaitBrain.h"
#include "brain/ModelBrain.h"

using namespace std;

namespace Crawler {

static unsigned int caller = 0;
static Robot* robot = nullptr;

PacketsComm::PacketsComm(){}

static void OnRequestLegAngles(void* caller, Packet& packet){

    auto request = (PacketRespondLegAngles*)(&packet);
    auto response = std::make_shared<PacketRespondLegAngles>();

    response->flags = request->flags;
    for(uint8_t legId : request->legIds){
        if(legId >= robot->legs.size()){
            LogError("PacketsComm", iLog << "invalid leg id=" << legId << " in RequestLegAnglesHandler");
            return;
        }
        response->legIds.push_back(legId);
    }
    if((request->flags>>0)&1){
        for(uint8_t legId : request->legIds){
            Leg* leg = robot->legs[legId];
            for(int i = 0; i < 4; i++){
                Joint* joint = leg->joints[i];
                response->targetAngle.push_back(joint->currentTargetAngle);
            }
        }
    }
    if((request->flags>>1)&1){
        for(uint8_t legId : request->legIds){
            Leg* leg = robot->legs[legId];
            for(int i = 0; i < 4; i++){
                Joint* joint = leg->joints[i];
                response->measuredAngle.push_back(joint->measuredAngle);
            }
        }
    }
    if((request->flags>>2)&1){
        for(uint8_t legId : request->legIds){
            Leg* leg = robot->legs[legId];
            for(int i = 0; i < 4; i++){
                Joint* joint = leg->joints[i];
                response->targetXYZ.push_back(joint->currentTargetXYZ);
            }
        }
    }
    if((request->flags>>3)&1){
        for(uint8_t legId : request->legIds){
            Leg* leg = robot->legs[legId];
            for(int i = 0; i < 4; i++){
                Joint* joint = leg->joints[i];
                response->measuredXYZ.push_back(joint->measuredXYZ);
            }
        }
    }
    ClientManager::SendPacket(response, packet.clientId);
}

static void OnRequestLegData(void* caller, Packet& packet){

    // LogDebug("PacketsComm", "RequestLegDataHandler()");
    auto request = (PacketRequestLegData*)(&packet);
    auto response = std::make_shared<PacketRespondLegData>();

    for(uint8_t legId : request->legIds){
        if(legId >= robot->legs.size()){
            LogError("PacketsComm", iLog << "invalid leg id=" << legId << " in RequestLegDataHandler");
            return;
        }
        response->legIds.push_back(legId);
        Leg* leg = robot->legs[legId];
        for(int i = 0; i < 4; i++){
            Joint* joint = leg->joints[i];
            response->targetAngle.push_back(joint->currentTargetAngle);
            response->measuredAngle.push_back(joint->measuredAngle);
            response->current.push_back(joint->measuredCurrent);
            response->pwm.push_back(joint->measuredPwm);
            response->temperature.push_back(joint->measuredTemperature);
            response->voltage.push_back(joint->measuredVoltage);
            response->statusDetail.push_back(joint->statusDetail);
            response->statusError.push_back(joint->statusError);
        }
        response->distance.push_back(leg->measuredDistance.value);
        response->weight.push_back(leg->measuredWeight.value);
    }
    ClientManager::SendPacket(response, packet.clientId);
}

static void OnRequestIMU(void* caller, Packet& packet){
    // LogDebug("PacketsComm", "RequestLegIMUHandler()");
    auto request = (PacketRequestIMUData*)(&packet);
    auto response = std::make_shared<PacketRespondIMUData>();
    response->acceleration.push_back(robot->imu_acc[0]); // acceleration x 
    response->acceleration.push_back(robot->imu_acc[1]); // acceleration y
    response->acceleration.push_back(robot->imu_acc[2]); // acceleration z
    response->up.push_back(robot->imu_up[0]); // up x
    response->up.push_back(robot->imu_up[1]); // up y
    response->up.push_back(robot->imu_up[2]); // up z
    response->gyro.push_back(robot->imu_gyro[0]); // gyro x
    response->gyro.push_back(robot->imu_gyro[1]); // gyro y
    response->gyro.push_back(robot->imu_gyro[2]); // gyro z
    ClientManager::SendPacket(response, packet.clientId);
}

static void OnMessageTest(void* caller, PacketMessage& packet){
    LogDebug("PacketsComm", iLog << "message = " << packet.message);
    for(auto& x : packet.params){
        LogDebug("PacketsComm", iLog << x.first << " = " << x.second);
    }
}

static void OnMessageGetCalib(void* caller, PacketMessage& packet){

    // parse request
    std::vector<unsigned int> jointIds;
    unsigned int numJoints = packet.GetInt("n");
    for(unsigned int i = 0; i < numJoints; i++){
        unsigned int jointId = packet.GetInt("j-"+to_string(i));
        jointIds.push_back(jointId);
    }

    // write response
    auto response = std::make_shared<PacketMessage>("respondCalib");
    response->AddInt("n", numJoints);
    for(unsigned int i = 0; i < numJoints; i++){
        unsigned int jointId = jointIds[i];
        if(jointId >= App::robot->jointsList.size()){
            ClientManager::SendLogError("PacketsComm", iLog << "OnMessageGetCalib() jointId " << jointId << " is invalid");
            continue;
        }
        unsigned int values[3];
        float angles[3];
        robot->jointsList[jointId]->GetCalibrationValues(values);
        robot->jointsList[jointId]->GetCalibrationAngles(angles);
        response->AddInt("j-"+to_string(i), jointId); // joint id
        response->AddInt("l-"+to_string(jointId), values[0]); // low 
        response->AddInt("m-"+to_string(jointId), values[1]); // mid
        response->AddInt("h-"+to_string(jointId), values[2]); // high
        response->AddFloat("cl-"+to_string(jointId), angles[0]); // low 
        response->AddFloat("cm-"+to_string(jointId), angles[1]); // mid
        response->AddFloat("ch-"+to_string(jointId), angles[2]); // high
    }
    ClientManager::SendPacket(response, packet.clientId);
}

static void OnMessageSetTorque(void* caller, PacketMessage& packet){
    bool torque = packet.GetBool("torque");
    ClientManager::SendLogInfo("PacketsComm", iLog << "torque was set to '" << (torque ? "on" : "off") << "'");
    if(torque){
        App::robot->TorqueOn(true);
    } else {
        App::robot->TorqueOff(true);
    }
}

static void OnMessagePrintStatus(void* caller, PacketMessage& packet){
    Robot* robot = App::robot;
    char buffer[200]; 
    for(Leg* leg : robot->legs){
        sprintf(buffer, "%s | (torque) %-3s %-3s %-3s %-3s | (status) %-9s %-9s %-9s %-9s", 
            leg->name.c_str(), 
            leg->joints[0]->torque ? "on" : "off",
            leg->joints[1]->torque ? "on" : "off",
            leg->joints[2]->torque ? "on" : "off",
            leg->joints[3]->torque ? "on" : "off",
            ServoStateToString(leg->joints[0]->servoState)->c_str(),
            ServoStateToString(leg->joints[1]->servoState)->c_str(),
            ServoStateToString(leg->joints[2]->servoState)->c_str(),
            ServoStateToString(leg->joints[3]->servoState)->c_str()
        );
        ClientManager::SendLogInfo("PacketsComm", iLog << buffer);
    }
}

static std::string GetRobotBrainName(Brain* brain){
    std::string name = brain->name;
    if(name == "surf"){
        SurferBrain* surferBrain = (SurferBrain*) brain;
        if(surferBrain->surfMode == 0){
            name += "-2";
        } else if(surferBrain->surfMode == 1){
            name += "-1";
        }
    }
    return name;
}

static void OnMessageSetBrain(void* caller, PacketMessage& packet){
    string brainName = packet.GetString("name");

    if(brainName == "calib"){
        robot->SetBrain(new CalibrationBrain());
    } else if(brainName == "walk"){
        WalkerBrain* brain = new WalkerBrain();
        robot->SetBrain(brain);
    } else if(brainName == "surf-1"){
        SurferBrain* brain = new SurferBrain();
        if(robot->brain->name == "surf"){
            brain->startupOnInit = false;
        }
        brain->surfMode = 1;
        robot->SetBrain(brain);
    } else if(brainName == "surf-2"){
        SurferBrain* brain = new SurferBrain();
        if(robot->brain->name == "surf"){
            brain->startupOnInit = false;
        }
        brain->surfMode = 0;
        robot->SetBrain(brain);
    } else if(brainName == "dance"){
        robot->SetBrain(new GaitBrain());
    } else if(brainName == "model"){
        robot->SetBrain(new ModelBrain());
    } else {
        ClientManager::SendLogInfo("PacketsComm", iLog << "invalid brain name: " << brainName);
    }

    if(robot->newBrain != nullptr){
        ClientManager::SendLogInfo("PacketsComm", iLog << "brain is now: " << GetRobotBrainName(robot->newBrain));
    }

}

static void OnMessageGetBrain(void* caller, PacketMessage& packet){
    auto response = std::make_shared<PacketMessage>("getBrain");
    std::string name = GetRobotBrainName(robot->brain);
    response->AddString("brain", name.c_str());
    ClientManager::SendPacket(response, packet.clientId);   
}

static void OnMessagePrintBrain(void* caller, PacketMessage& packet){
    std::string name = GetRobotBrainName(robot->brain);
    ClientManager::SendLogInfo("PacketsComm", iLog << "current brain is " << name);
}

static void OnMessagePrintAngles(void* caller, PacketMessage& packet){
    Robot* robot = App::robot;
    char buffer[200];
    for(Leg* leg : robot->legs){
        sprintf(buffer, "%s target:   %6.1f %6.1f %6.1f %6.1f (deg)", 
            leg->name.c_str(), 
            leg->joints[0]->currentTargetAngle * RAD_2_DEGf,
            leg->joints[1]->currentTargetAngle * RAD_2_DEGf,
            leg->joints[2]->currentTargetAngle * RAD_2_DEGf,
            leg->joints[3]->currentTargetAngle * RAD_2_DEGf
        );
        ClientManager::SendLogInfo("PacketsComm", iLog << buffer);
    }
    for(Leg* leg : robot->legs){
        sprintf(buffer, "%s measured: %6.1f %6.1f %6.1f %6.1f (deg)", 
            leg->name.c_str(), 
            leg->joints[0]->measuredAngle.value * RAD_2_DEGf,
            leg->joints[1]->measuredAngle.value * RAD_2_DEGf,
            leg->joints[2]->measuredAngle.value * RAD_2_DEGf,
            leg->joints[3]->measuredAngle.value * RAD_2_DEGf
        );
        ClientManager::SendLogInfo("PacketsComm", iLog << buffer);
    }
}

static void OnMessagePrintPositions(void* caller, PacketMessage& packet){
    Robot* robot = App::robot;
    char buffer[200];
    ClientManager::SendLogInfo("PacketsComm", "foot positions (mm) using target angles:");
    for(Leg* leg : robot->legs){
        float angles[4] = {
            leg->joints[0]->currentTargetAngle,
            leg->joints[1]->currentTargetAngle,
            leg->joints[2]->currentTargetAngle,
            leg->joints[3]->currentTargetAngle
        };
        leg->FKJoints(angles);
        sprintf(buffer, "%s | (joint) %4.0f %4.0f %4.0f | (body) %4.0f %4.0f %4.0f", 
            leg->name.c_str(), 
            leg->fkJointsResult.jointPositions[4][0]*1000.0f,
            leg->fkJointsResult.jointPositions[4][1]*1000.0f,
            leg->fkJointsResult.jointPositions[4][2]*1000.0f,
            (leg->hipTransform * leg->fkJointsResult.jointPositions[4])[0]*1000.0f,
            (leg->hipTransform * leg->fkJointsResult.jointPositions[4])[1]*1000.0f,
            (leg->hipTransform * leg->fkJointsResult.jointPositions[4])[2]*1000.0f
        );
        ClientManager::SendLogInfo("PacketsComm", iLog << buffer);
    }
    ClientManager::SendLogInfo("PacketsComm", "foot positions (mm) using measured angles:");
    for(Leg* leg : robot->legs){
        float angles[4] = {
            leg->joints[0]->measuredAngle,
            leg->joints[1]->measuredAngle,
            leg->joints[2]->measuredAngle,
            leg->joints[3]->measuredAngle
        };
        leg->FKJoints(angles);
        sprintf(buffer, "%s | (joint) %4.0f %4.0f %4.0f | (body) %4.0f %4.0f %4.0f", 
            leg->name.c_str(), 
            leg->fkJointsResult.jointPositions[4][0]*1000.0f,
            leg->fkJointsResult.jointPositions[4][1]*1000.0f,
            leg->fkJointsResult.jointPositions[4][2]*1000.0f,
            (leg->hipTransform * leg->fkJointsResult.jointPositions[4])[0]*1000.0f,
            (leg->hipTransform * leg->fkJointsResult.jointPositions[4])[1]*1000.0f,
            (leg->hipTransform * leg->fkJointsResult.jointPositions[4])[2]*1000.0f
        );
        ClientManager::SendLogInfo("PacketsComm", iLog << buffer);
    }
}

static void OnMessageExit(void* caller, PacketMessage& packet){
    App::RequestExit("Remote");
}

static void OnMessageReboot(void* caller, PacketMessage& packet){
    ClientManager::SendLogInfo("PacketsComm", "reboot not implemented");
}

void PacketsComm::Init(Robot* robotPtr){
    robot = robotPtr;
    ClientManager::SubscribePacket(PacketType::RequestLegAngles, &caller, &OnRequestLegAngles);
    ClientManager::SubscribePacket(PacketType::RequestLegData, &caller, &OnRequestLegData);
    ClientManager::SubscribePacket(PacketType::RequestIMUData, &caller, &OnRequestIMU);
    ClientManager::SubscribeMessage("test", &caller, &OnMessageTest);
    ClientManager::SubscribeMessage("requestCalib", &caller, &OnMessageGetCalib);
    ClientManager::SubscribeMessage("setTorque", &caller, &OnMessageSetTorque);
    ClientManager::SubscribeMessage("setBrain", &caller, &OnMessageSetBrain);
    ClientManager::SubscribeMessage("getBrain", &caller, &OnMessageGetBrain);
    ClientManager::SubscribeMessage("printBrain", &caller, &OnMessagePrintBrain);
    ClientManager::SubscribeMessage("printStatus", &caller, &OnMessagePrintStatus);
    ClientManager::SubscribeMessage("printAngles", &caller, &OnMessagePrintAngles);
    ClientManager::SubscribeMessage("printPositions", &caller, &OnMessagePrintPositions);
    ClientManager::SubscribeMessage("exit", &caller, &OnMessageExit);
    ClientManager::SubscribeMessage("reboot", &caller, &OnMessageReboot);
}

void PacketsComm::Cleanup(){

}




}