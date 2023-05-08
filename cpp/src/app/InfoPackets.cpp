#include "InfoPackets.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include "App.h"
#include "remote/ClientManager.h"
#include "remote/Packet.h"
#include "core/Log.h"

using namespace std;

namespace Crawler {

static unsigned int caller = 0;
static Robot* robot = nullptr;

InfoPackets::InfoPackets(){}

static void OnRequestLegAngles(void* caller, Packet& packet){

    auto request = (PacketRespondLegAngles*)(&packet);
    auto response = std::make_shared<PacketRespondLegAngles>();

    response->flags = request->flags;
    for(uint8_t legId : request->legIds){
        if(legId >= robot->legs.size()){
            LogError("InfoPackets", iLog << "invalid leg id=" << legId << " in RequestLegAnglesHandler");
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

    // LogDebug("InfoPackets", "RequestLegDataHandler()");
    auto request = (PacketRequestLegData*)(&packet);
    auto response = std::make_shared<PacketRespondLegData>();

    for(uint8_t legId : request->legIds){
        if(legId >= robot->legs.size()){
            LogError("InfoPackets", iLog << "invalid leg id=" << legId << " in RequestLegDataHandler");
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
        response->distance.push_back(0.0f);
        response->weight.push_back(0.0f);
    }
    ClientManager::SendPacket(response, packet.clientId);
}

static void OnRequestLegIMU(void* caller, Packet& packet){
    // LogDebug("InfoPackets", "RequestLegIMUHandler()");
    auto request = (PacketRequestIMUData*)(&packet);
    auto response = std::make_shared<PacketRespondIMUData>();
    response->acceleration.push_back(0.0f); // acceleration x
    response->acceleration.push_back(0.0f); // acceleration y
    response->acceleration.push_back(0.0f); // acceleration z
    response->gyro.push_back(0.0f); // gyro x
    response->gyro.push_back(0.0f); // gyro y
    response->gyro.push_back(0.0f); // gyro z
    ClientManager::SendPacket(response, packet.clientId);
}

static void OnMessageTest(void* caller, PacketMessage& packet){
    LogDebug("InfoPackets", iLog << "message = " << packet.message);
    for(auto& x : packet.params){
        LogDebug("InfoPackets", iLog << x.first << " = " << x.second);
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
            ClientManager::SendLogError("InfoPackets", iLog << "OnMessageGetCalib() jointId " << jointId << " is invalid");
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
    ClientManager::SendLogInfo("InfoPackets", iLog << "torque was set to <" << (torque ? "on" : "off") << ">");
    if(torque){
        App::robot->TorqueOn(true);
    } else {
        App::robot->TorqueOff(true);
    }
}

void InfoPackets::Init(Robot* robotPtr){
    robot = robotPtr;
    ClientManager::SubscribePacket(PacketType::RequestLegAngles, &caller, &OnRequestLegAngles);
    ClientManager::SubscribePacket(PacketType::RequestLegData, &caller, &OnRequestLegData);
    ClientManager::SubscribePacket(PacketType::RequestIMUData, &caller, &OnRequestLegIMU);
    ClientManager::SubscribeMessage("test", &caller, &OnMessageTest);
    ClientManager::SubscribeMessage("requestCalib", &caller, &OnMessageGetCalib);
    ClientManager::SubscribeMessage("setTorque", &caller, &OnMessageSetTorque);
}

void InfoPackets::Cleanup(){

}




}