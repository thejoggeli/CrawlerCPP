#include "PacketType.h"

#include <stdio.h>

namespace Crawler {

static std::shared_ptr<std::string> PrettyPacketString(PacketType type, const char* name){
    char buffer[100];
    sprintf(buffer, "%s (0x%04X)", name, (uint16_t)type);
    return std::make_shared<std::string>(buffer);
}

std::shared_ptr<std::string> PacketTypeToString(PacketType type){
    switch(type){
        case PacketType::Invalid: 
            return PrettyPacketString(type, "Invalid");
        case PacketType::CS_GamepadKey: 
            return PrettyPacketString(type, "GamepadButton");
        case PacketType::CS_GamepadJoystick:
            return PrettyPacketString(type, "GamepadJoystick");
    }
    return PrettyPacketString(type, "PacketType");
}



}