#include "ByteBufferWriter.h"
#include "core/Log.h"
#include <stdexcept>
#include "util/Endian.h"
#include <cstring>

namespace Crawler {

ByteBufferWriter::ByteBufferWriter(std::vector<uint8_t>* bytes){
    this->bytes = bytes;
}

template<typename T>
void ByteBufferWriter::Write(T val){
    unsigned int s = bytes->size();
    bytes->resize(s + sizeof(T));
    *(T*)(bytes->data()+s) = val;
}

template<typename T>
void ByteBufferWriter::WriteVector(const std::vector<T>& vector){
    unsigned int s1 = bytes->size();
    unsigned int s2 = sizeof(T) * vector.size();
    bytes->resize(s1 + s2);
    memcpy(bytes->data()+s1, vector.data(), s2);
}

void ByteBufferWriter::WriteString(const char* str){
    unsigned int i = 0;
    while(str[i] != '\0'){
        bytes->push_back(str[i]);
        i++;
    }
    bytes->push_back('\0');
}

const uint8_t* ByteBufferWriter::GetBytes(){
    return bytes->data();
}

unsigned int ByteBufferWriter::GetSize(){
    return bytes->size();
}

template void ByteBufferWriter::Write<float>(float val);
template void ByteBufferWriter::Write<double>(double val);
template void ByteBufferWriter::Write<int8_t>(int8_t val);
template void ByteBufferWriter::Write<int16_t>(int16_t val);
template void ByteBufferWriter::Write<int32_t>(int32_t val);
template void ByteBufferWriter::Write<int64_t>(int64_t val);
template void ByteBufferWriter::Write<uint8_t>(uint8_t val);
template void ByteBufferWriter::Write<uint16_t>(uint16_t val);
template void ByteBufferWriter::Write<uint32_t>(uint32_t val);
template void ByteBufferWriter::Write<uint64_t>(uint64_t val);

template void ByteBufferWriter::WriteVector<float>(const std::vector<float>& vector);
template void ByteBufferWriter::WriteVector<double>(const std::vector<double>& vector);
template void ByteBufferWriter::WriteVector<int8_t>(const std::vector<int8_t>& vector);
template void ByteBufferWriter::WriteVector<int16_t>(const std::vector<int16_t>& vector);
template void ByteBufferWriter::WriteVector<int32_t>(const std::vector<int32_t>& vector);
template void ByteBufferWriter::WriteVector<int64_t>(const std::vector<int64_t>& vector);
template void ByteBufferWriter::WriteVector<uint8_t>(const std::vector<uint8_t>& vector);
template void ByteBufferWriter::WriteVector<uint16_t>(const std::vector<uint16_t>& vector);
template void ByteBufferWriter::WriteVector<uint32_t>(const std::vector<uint32_t>& vector);
template void ByteBufferWriter::WriteVector<uint64_t>(const std::vector<uint64_t>& vector);

}