#pragma once

#include <cstdint>
#include <memory>

namespace Crawler {

class Packet;

enum class PacketType : uint16_t {
    // 0x000X
    Invalid = 0x0000,
    Message = 0x0008,
    // 0x001X
    GamepadKey = 0x0010,
    GamepadJoystick = 0x0011,
    // 0x002X
    // 0x003X
    RequestSetJointPosition = 0x0020,
    RequestSetLegAngles = 0x0021,
    // 0x003X
    RequestLegData = 0x0030,
    RespondLegData = 0x0031,
    RequestLegAngles = 0x0032,
    RespondLegAngles = 0x0033,
    // 0x004X
    RequestIMUData = 0x0040,
    RespondIMUData = 0x0041,
};

std::shared_ptr<std::string> PacketTypeToString(PacketType type);

std::shared_ptr<Packet> CreatePacketFromType(PacketType type);

}