#pragma once

#include <cstdint>
#include <memory>

namespace Crawler {

enum class PacketType : uint16_t {
    Invalid = 0x0000,
    GamepadKey = 0x0010,
    GamepadJoystick = 0x0011,
};

std::shared_ptr<std::string> PacketTypeToString(PacketType type);

}