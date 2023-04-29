#pragma once

#include <cstdint>
#include <vector>

namespace Crawler {

class ByteBufferWriter {

private:

    std::vector<uint8_t>* bytes;

public:

    int errors = 0;
    bool swapEndian = false;

    ByteBufferWriter(std::vector<uint8_t>* bytes);

    template<typename T>
    void Write(T val);

    template<typename T>
    void WriteVector(const std::vector<T>& vector);

    void WriteString(const char* str);

	const uint8_t* GetBytes();
	unsigned int GetSize();

};

}