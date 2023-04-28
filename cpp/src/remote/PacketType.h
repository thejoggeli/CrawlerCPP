#pragma once

#include <cstdint>
#include <memory>

namespace Crawler {

enum class PacketType : uint16_t {
    Invalid = 0x0000,

    CS_GamepadKey = 0x0010,
    CS_GamepadJoystick = 0x0011,

    CS_RequestLegData = 0x0030,
    SC_RespondLegData = 0x0031,

    CS_RequestIMUData = 0x0040,
    SC_RespondIMUData = 0x0041,
};

std::shared_ptr<std::string> PacketTypeToString(PacketType type);

}