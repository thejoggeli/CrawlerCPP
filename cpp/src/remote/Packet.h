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

    Packet(const uint8_t* bytes, unsigned int size);
    Packet(PacketType type);

private:

    void Unpack(const uint8_t* bytes, unsigned int size);

};

}