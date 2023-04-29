#include "PacketType.h"

#include <stdio.h>
#include "Packet.h"

#define PACKET_TYPE_TO_STRING(name) \
case PacketType::name: return PrettyPacketString(type, "name") \

#define CREATE_PACKET_FROM_TYPE(type) \
case PacketType::type: return std::make_shared<Packet##type>() \

namespace Crawler {

static std::shared_ptr<std::string> PrettyPacketString(PacketType type, const char* name){
    char buffer[100];
    sprintf(buffer, "%s (0x%04X)", name, (uint16_t)type);
    return std::make_shared<std::string>(buffer);
}

std::shared_ptr<std::string> PacketTypeToString(PacketType type){
    switch(type){
        PACKET_TYPE_TO_STRING(Invalid);
        PACKET_TYPE_TO_STRING(Message);
        PACKET_TYPE_TO_STRING(GamepadKey);
        PACKET_TYPE_TO_STRING(GamepadJoystick);
        PACKET_TYPE_TO_STRING(RequestSetJointPosition);
        PACKET_TYPE_TO_STRING(RequestSetLegAngles);
        PACKET_TYPE_TO_STRING(RequestLegData);
        PACKET_TYPE_TO_STRING(RespondLegData);
        PACKET_TYPE_TO_STRING(RequestLegAngles);
        PACKET_TYPE_TO_STRING(RespondLegAngles);
        PACKET_TYPE_TO_STRING(RequestIMUData);
        PACKET_TYPE_TO_STRING(RespondIMUData);
    }
    return PrettyPacketString(type, "PacketType");
}

std::shared_ptr<Packet> CreatePacketFromType(PacketType type){
    std::shared_ptr<Packet> packet = nullptr;
    switch(type){
        CREATE_PACKET_FROM_TYPE(Message);
        CREATE_PACKET_FROM_TYPE(GamepadKey);
        CREATE_PACKET_FROM_TYPE(GamepadJoystick);
        CREATE_PACKET_FROM_TYPE(RequestSetJointPosition);
        CREATE_PACKET_FROM_TYPE(RequestSetLegAngles);
        CREATE_PACKET_FROM_TYPE(RequestLegData);
        CREATE_PACKET_FROM_TYPE(RespondLegData);
        CREATE_PACKET_FROM_TYPE(RequestLegAngles);
        CREATE_PACKET_FROM_TYPE(RespondLegAngles);
        CREATE_PACKET_FROM_TYPE(RequestIMUData);
        CREATE_PACKET_FROM_TYPE(RespondIMUData);
    }
    return nullptr;
}


}