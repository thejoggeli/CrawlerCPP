#pragma once

#include <string>
#include "PacketType.h"
#include "util/SmartBuffer.h"

namespace Crawler {

class Packet {

private:

public:

    int clientId = -1;

    PacketType type = PacketType::Invalid;
    SmartBuffer data;

    Packet(PacketType type);
    static std::shared_ptr<Packet> Create(PacketType type);
    static std::shared_ptr<Packet> Unpack(const uint8_t* bytes, unsigned int size, int clientId = -1);

private:

};

}