#include "SmartBuffer.h"
#include "core/Log.h"

namespace Crawler {

SmartBuffer::SmartBuffer(unsigned int reserveBytes){
    bytes.reserve(reserveBytes);
}

SmartBuffer::SmartBuffer(const uint8_t* bytes, unsigned int numBytes){
    SetBytes(bytes, numBytes);
}

SmartBuffer::~SmartBuffer(){

}

void SmartBuffer::AddString(const std::string& key, const char* value){
    if(!map_str.contains(key)){
        map_str[key] = this->bytes.size();
    }
    AddString(value);
}

void SmartBuffer::AddString(unsigned int key, const char* value){
    if(!map_int.contains(key)){
        map_int[key] = this->bytes.size();
    }
    AddString(value);
}

void SmartBuffer::AddString(const char* value){
    unsigned int i = 0;
    while(true){
        bytes.push_back(value[i]);
        if(value[i] == '\0'){
            return;
        }
        i++;
    }
}

template<typename ValType>
void SmartBuffer::Add(const std::string& key, ValType value){
    if(!map_str.contains(key)){
        map_str[key] = this->bytes.size();
        LogDebug("SmartBuffer", iLog << "adding key " << key);
    }
    Add<ValType>(value);
}

template<typename ValType>
void SmartBuffer::Add(unsigned int key, ValType value){
    if(!map_int.contains(key)){
        map_int[key] = this->bytes.size();
        LogDebug("SmartBuffer", iLog << "adding key " << key);
    }
    Add<ValType>(value);
}

template<typename ValType>
void SmartBuffer::Add(ValType value){
    uint8_t* bytes = (uint8_t*)(&value);
    for(int i = 0; i < sizeof(ValType); i++){
        this->bytes.push_back(bytes[i]);
    }
}

const char* SmartBuffer::GetString(const std::string& key){
    if(!map_str.contains(key)){
        LogError("SmartBuffer", iLog << "key is not in map_str: " << key);
        return nullptr;
    }
    unsigned int bytePos = map_str[key];
    return (char*)&(bytes[bytePos]);
}

const char* SmartBuffer::GetString(unsigned int key){
    if(!map_int.contains(key)){
        LogError("SmartBuffer", iLog << "key is not in map_int: " << key);
        return nullptr;
    }
    unsigned int bytePos = map_int[key];
    return (char*)&(bytes[bytePos]);
}

template<typename ValType>
ValType SmartBuffer::Get(const std::string& key, unsigned int offset){
    if(!map_str.contains(key)){
        LogError("SmartBuffer", iLog << "key is not in map_str: " << key);
        return 0;
    }
    unsigned int bytePos = map_str[key];
    uint8_t* bytes = &(this->bytes.data()[bytePos+offset*sizeof(ValType)]);
    return *((ValType*)bytes);
}

template<typename ValType>
ValType SmartBuffer::Get(unsigned int key, unsigned int offset){
    if(!map_int.contains(key)){
        LogError("SmartBuffer", iLog << "key is not in map_int: " << key);
        return 0;
    }
    unsigned int bytePos = map_int[key];
    uint8_t* bytes = &(this->bytes.data()[bytePos+offset*sizeof(ValType)]);
    return *((ValType*)bytes);
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