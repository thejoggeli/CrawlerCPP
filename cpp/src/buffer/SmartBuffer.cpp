#include "SmartBuffer.h"
#include "core/Log.h"
#include "util/Endian.h"

namespace Crawler {

SmartBuffer::SmartBuffer(unsigned int reserveBytes){
    bytes.reserve(reserveBytes);
}

SmartBuffer::SmartBuffer(const uint8_t* bytes, unsigned int numBytes){
    SetBytes(bytes, numBytes);
}

SmartBuffer::~SmartBuffer(){

}

void SmartBuffer::PrintPretty(bool hex, int lineSize){
    const char* format = hex ? "%02X " : "%03d ";
    std::string line = "";
    for(int i = 0; i < bytes.size(); i++){
        char buffer[10];
        if(i%lineSize==0){
            if(line.length() > 0){
                LogInfo("SmartBuffer", iLog << line);
                line = "";
            }
            sprintf(buffer, "%04d | ", i);
            line += buffer;
        }
        sprintf(buffer, format, bytes[i]);
        line += buffer;
    }
    if(line.length() > 0){
        LogInfo("SmartBuffer", iLog << line);
    }
}

std::vector<std::string>* SmartBuffer::GetStringKeys(){
    return &keys_str;
}

std::vector<unsigned int>* SmartBuffer::GetIntKeys(){
    return &keys_int;
}

void SmartBuffer::AddString(const std::string& key, const char* value){
    if(!map_str.contains(key)){
        map_str[key] = this->bytes.size();
        keys_str.push_back(key);
    }
    AddString(value);
}

void SmartBuffer::AddString(unsigned int key, const char* value){
    if(!map_int.contains(key)){
        map_int[key] = this->bytes.size();
        keys_int.push_back(key);
    }
    AddString(value);
}

void SmartBuffer::AddString(const char* value){
    unsigned int i = 0;
    while(value[i] != '\0'){
        bytes.push_back(value[i]);
        i++;
    }
    bytes.push_back('\0');
}

template<typename ValType>
void SmartBuffer::Add(const std::string& key, ValType value){
    if(!map_str.contains(key)){
        map_str[key] = this->bytes.size();
        keys_str.push_back(key);
        // LogDebug("SmartBuffer", iLog << "adding key " << key);
    }
    Add<ValType>(value);
}

template<typename ValType>
void SmartBuffer::Add(unsigned int key, ValType value){
    if(!map_int.contains(key)){
        map_int[key] = this->bytes.size();
        keys_int.push_back(key);
        // LogDebug("SmartBuffer", iLog << "adding key " << key);
    }
    Add<ValType>(value);
}

template<typename ValType>
void SmartBuffer::Add(ValType value){
    if(swapEndian){
        value = SwapEndian(value);
    }
    unsigned int s = bytes.size();
    bytes.resize(s + sizeof(ValType));
    *(ValType*)(bytes.data()+s) = value;
}

const char* SmartBuffer::GetString(const std::string& key){
    if(!map_str.contains(key)){
        errors += 1;
        LogError("SmartBuffer", iLog << "key is not in map_str: " << key);
        return nullptr;
    }
    unsigned int bytePos = map_str[key];
    return (char*)(bytes.data()+bytePos);
}

const char* SmartBuffer::GetString(unsigned int key){
    if(!map_int.contains(key)){
        errors += 1;
        LogError("SmartBuffer", iLog << "key is not in map_int: " << key);
        return nullptr;
    }
    unsigned int bytePos = map_int[key];
    return (char*)(bytes.data()+bytePos);
}

template<typename ValType>
ValType SmartBuffer::Get(const std::string& key, unsigned int offset){
    if(!map_str.contains(key)){
        errors += 1;
        LogError("SmartBuffer", iLog << "key is not in map_str: " << key);
        return 0;
    }
    unsigned int bytePos = map_str[key];
    uint8_t* bytes = &(this->bytes.data()[bytePos+offset*sizeof(ValType)]);
    ValType value = *((ValType*)bytes);
    if(swapEndian){
        value = SwapEndian<ValType>(value);
    }
    return value;
}

template<typename ValType>
ValType SmartBuffer::Get(unsigned int key, unsigned int offset){
    if(!map_int.contains(key)){
        errors += 1;
        LogError("SmartBuffer", iLog << "key is not in map_int: " << key);
        return 0;
    }
    unsigned int bytePos = map_int[key];
    uint8_t* bytes = &(this->bytes.data()[bytePos+offset*sizeof(ValType)]);
    ValType value = *((ValType*)bytes);
    if(swapEndian){
        value = SwapEndian<ValType>(value);
    }
    return value;
}

void SmartBuffer::SetBytes(const uint8_t* bytes, unsigned int numBytes){
    this->bytes = std::vector<uint8_t>(bytes, bytes+numBytes);
}

const uint8_t* SmartBuffer::GetBytes(){
    return this->bytes.data();
}

unsigned int SmartBuffer::GetSize(){
    return this->bytes.size();
}


#define SMART_BUFFER_ALLOW_TYPE(T) \
template void SmartBuffer::Add<T>(unsigned int key, T value); \
template void SmartBuffer::Add<T>(const std::string& key, T value); \
template void SmartBuffer::Add<T>(T value); \
template T SmartBuffer::Get<T>(unsigned int key, unsigned int offset); \
template T SmartBuffer::Get<T>(const std::string& key, unsigned int offset); \

SMART_BUFFER_ALLOW_TYPE(float)
SMART_BUFFER_ALLOW_TYPE(double)
SMART_BUFFER_ALLOW_TYPE(uint8_t)
SMART_BUFFER_ALLOW_TYPE(uint16_t)
SMART_BUFFER_ALLOW_TYPE(uint32_t)
SMART_BUFFER_ALLOW_TYPE(uint64_t)
SMART_BUFFER_ALLOW_TYPE(int8_t)
SMART_BUFFER_ALLOW_TYPE(int16_t)
SMART_BUFFER_ALLOW_TYPE(int32_t)
SMART_BUFFER_ALLOW_TYPE(int64_t)

}