#include "InfoPackets.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include "App.h"
#include "remote/ClientManager.h"
#include "remote/Packet.h"
#include "util/SmartBuffer.h"
#include "core/Log.h"

namespace Crawler {

static unsigned int caller = 0;
static Robot* robot = nullptr;

static void CS_RequestLegDataHandler(void* caller, Packet& packet){

    // LogDebug("InfoPackets", "CS_RequestLegDataHandler()");

    std::shared_ptr<Packet> response = Packet::Create(PacketType::SC_RespondLegData);
    
    uint8_t dataType = packet.data.Get<uint8_t>("dataType");
    uint8_t numLegs = packet.data.Get<uint8_t>("numLegs");

    response->data.Add<uint8_t>(dataType);
    response->data.Add<uint8_t>(numLegs);

    // LogDebug("InfoPackets", iLog << "CS_RequestLegDataHandler dataType=" << (int)dataType);
    // LogDebug("InfoPackets", iLog << "CS_RequestLegDataHandler numLegs=" << (int)numLegs);

    for(uint8_t i = 0; i < numLegs; i++){
        uint8_t legId = packet.data.Get<uint8_t>("legId", i);
        if(legId >= robot->legs.size()){
            LogError("InfoPackets", iLog << "invalid leg id=" << legId << " in CS_RequestLegDataHandler");
            return;
        }
        response->data.Add<uint8_t>(legId);
        Leg* leg = robot->legs[legId];
        for(int j = 0; j < leg->joints.size(); j++){
            Joint* joint = leg->joints[j];
            switch(dataType){
                case 0:
                    response->data.Add<float>(joint->measuredAngle);
                    break;
                case 1:
                    response->data.Add<float>(joint->currentTargetAngle);
                    response->data.Add<float>(joint->measuredAngle);
                    response->data.Add<float>(joint->measuredCurrent);
                    response->data.Add<float>(joint->measuredPwm);
                    response->data.Add<float>(joint->measuredTemperature);
                    response->data.Add<float>(joint->measuredVoltage);
                    response->data.Add<uint8_t>(joint->statusDetail);
                    response->data.Add<uint8_t>(joint->statusError);
                    break;
                default:
                    LogError("InfoPackets", iLog << "invalid dataType=" << dataType << " in CS_RequestLegDataHandler");
                    return;
            }
        }
        switch(dataType){
            case 1:
                response->data.Add<float>(0.0f); // distance
                response->data.Add<float>(0.0f); // weight
                break;
        }
    }
    ClientManager::SendPacket(response, packet.clientId);
}

static void CS_RequestLegIMUHandler(void* caller, Packet& packet){

    // LogDebug("InfoPackets", "CS_RequestLegIMUHandler()");

    std::shared_ptr<Packet> response = Packet::Create(PacketType::SC_RespondIMUData);
    response->data.Add<float>(0.0f); // acceleration x
    response->data.Add<float>(0.0f); // acceleration y
    response->data.Add<float>(0.0f); // acceleration z
    response->data.Add<float>(0.0f); // gyro x
    response->data.Add<float>(0.0f); // gyro y
    response->data.Add<float>(0.0f); // gyro z
    ClientManager::SendPacket(response, packet.clientId);
}

InfoPackets::InfoPackets(){}

void InfoPackets::Init(Robot* robotPtr){
    robot = robotPtr;
    ClientManager::SubscribePacket(PacketType::CS_RequestLegData, &caller, &CS_RequestLegDataHandler);
    ClientManager::SubscribePacket(PacketType::CS_RequestIMUData, &caller, &CS_RequestLegIMUHandler);
}

void InfoPackets::Cleanup(){

}




}