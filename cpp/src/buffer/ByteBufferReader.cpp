#include "ByteBufferReader.h"
#include "core/Log.h"
#include <stdexcept>
#include "util/Endian.h"
#include <cstring>

namespace Crawler {

ByteBufferReader::ByteBufferReader(const uint8_t* bytes, unsigned int size) : bytes(bytes), size(size){

}

template<typename T>
T ByteBufferReader::Read(){
    if(readPtr + sizeof(T) > size){
        errors += 1;
        LogError("ByteBufferReader", iLog << "readPtr overflow " << "readPtr=" << readPtr << ", size=" << size);
        return 0;
    }
    T val = ((T*)(bytes+readPtr))[0];
    if(swapEndian){
        val = SwapEndian<T>(val);
    }
    readPtr += sizeof(T);
    return val;
}

const char* ByteBufferReader::ReadString(){
    const char* str = (char*)(bytes+readPtr);
    readPtr += std::strlen(str)+1;
    if(readPtr > size){
        errors += 1;
        return nullptr;
    }
    return str;
}

const uint8_t* ByteBufferReader::GetBytes(){
    return bytes;
}

unsigned int ByteBufferReader::GetSize(){
    return size;
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