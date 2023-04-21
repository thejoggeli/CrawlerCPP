#include "ByteBufferReader.h"
#include "core/Log.h"
#include <stdexcept>

namespace Crawler {

ByteBufferReader::ByteBufferReader(const uint8_t* bytes, unsigned int size) : bytes(bytes), size(size){

}

template<typename T>
T ByteBufferReader::Read(){
    if(readPtr + sizeof(T) > size){
        LogError("ByteBufferReader", iLog << "readPtr overflow " << "readPtr=" << readPtr << ", size=" << size);
        error = 1;
        return 0;
    }
    T val = ((T*)(bytes+readPtr))[0];
    readPtr += sizeof(T);
    return val;
}


template float ByteBufferReader::Read<float>();
template double ByteBufferReader::Read<double>();
template int8_t ByteBufferReader::Read<int8_t>();
template int16_t ByteBufferReader::Read<int16_t>();
template int32_t ByteBufferReader::Read<int32_t>();
template int64_t ByteBufferReader::Read<int64_t>();
template uint8_t ByteBufferReader::Read<uint8_t>();
template uint16_t ByteBufferReader::Read<uint16_t>();
template uint32_t ByteBufferReader::Read<uint32_t>();
template uint64_t ByteBufferReader::Read<uint64_t>();

}