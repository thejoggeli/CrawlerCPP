#pragma once

#include <cstdint>

namespace Crawler {

class ByteBufferReader {

public:

    const uint8_t* bytes = nullptr;
    const unsigned int size;
    int readPtr = 0;
    int errors = 0;
    bool swapEndian = false;

    ByteBufferReader(const uint8_t* bytes, unsigned int size);

    template<typename T>
    T Read();
    const char* ReadString();

};

}