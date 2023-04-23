#pragma once

#include <cstdint>
#include <memory>

namespace Crawler {

enum class PacketType : uint16_t {
    Invalid = 0x0000,
    CS_GamepadKey = 0x0010,
    CS_GamepadJoystick = 0x0011,
    SC_ReadRobotPosLegAngles = 0x0020,
};

std::shared_ptr<std::string> PacketTypeToString(PacketType type);

}