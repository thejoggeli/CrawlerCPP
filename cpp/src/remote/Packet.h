#pragma once

#include <string>
#include "PacketType.h"
#include "util/SmartBuffer.h"

namespace Crawler {

class Packet {

public:

    int clientId = -1;

    PacketType type = PacketType::Invalid;
    SmartBuffer data;

    // Packet(const uint8_t* bytes, unsigned int size);
    Packet(PacketType type);

    static std::shared_ptr<Packet> Unpack(const uint8_t* bytes, unsigned int size);

private:

};

}