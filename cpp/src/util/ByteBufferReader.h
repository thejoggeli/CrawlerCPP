#pragma once

#include <cstdint>

namespace Crawler {

class ByteBufferReader {

public:

    const uint8_t* bytes = nullptr;
    const unsigned int size;
    int readPtr = 0;
    int error = 0;

    ByteBufferReader(const uint8_t* bytes, unsigned int size);

    template<typename T>
    T Read();

};

}