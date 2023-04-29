#include "InfoPackets.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include "App.h"
#include "remote/ClientManager.h"
#include "remote/Packet.h"
#include "core/Log.h"

namespace Crawler {

static unsigned int caller = 0;
static Robot* robot = nullptr;

static void RequestLegAnglesHandler(void* caller, Packet& packet){

    auto request = (PacketRespondLegAngles*)(&packet);
    auto response = std::make_shared<PacketRespondLegAngles>();

    for(uint8_t legId : request->legIds){
        if(legId >= robot->legs.size()){
            LogError("InfoPackets", iLog << "invalid leg id=" << legId << " in RequestLegAnglesHandler");
            return;
        }
        response->legIds.push_back(legId);
        Leg* leg = robot->legs[legId];
        for(int i = 0; i < 4; i++){
            Joint* joint = leg->joints[i];
            response->targetAngle.push_back(joint->currentTargetAngle);
            response->measuredAngle.push_back(joint->measuredAngle);
        }
    }
    ClientManager::SendPacket(response, packet.clientId);
}

static void RequestLegDataHandler(void* caller, Packet& packet){

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

static void RequestLegIMUHandler(void* caller, Packet& packet){
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

static void PacketMessageTestHandler(void* caller, PacketMessage& packet){

    LogDebug("InfoPackets", iLog << "message = " << packet.message);
    for(auto& x : packet.params){
        LogDebug("InfoPackets", iLog << x.first << " = " << x.second);
    }

}

InfoPackets::InfoPackets(){}

void InfoPackets::Init(Robot* robotPtr){
    robot = robotPtr;
    ClientManager::SubscribePacket(PacketType::RequestLegAngles, &caller, &RequestLegAnglesHandler);
    ClientManager::SubscribePacket(PacketType::RequestLegData, &caller, &RequestLegDataHandler);
    ClientManager::SubscribePacket(PacketType::RequestIMUData, &caller, &RequestLegIMUHandler);
    ClientManager::SubscribePacketMessage("test", &caller, &PacketMessageTestHandler);
}

void InfoPackets::Cleanup(){

}




}