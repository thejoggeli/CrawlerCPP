#pragma once

#include <cstdint>

namespace Crawler {

class PacketParser {

    static void Pack();
    static void Unpack(const uint8_t* bytes, unsigned int numBytes);

};

}