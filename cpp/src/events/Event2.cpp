#include "Event2.h"

namespace Crawler {

Event2::Event2(){
    bytes.reserve(64);
}

Event2::Event2(const uint8_t* bytes, unsigned int numBytes){
    SetBytes(bytes, numBytes);
}

Event2::~Event2(){

}

void Event2::AddString(const char* key, const char* value){
    unsigned int bytePos = this->bytes.size();
    map[std::string(key)] = bytePos;
    unsigned int i = 0;
    while(true){
        bytes.push_back(value[i]);
        if(value[i] == '\0'){
            return;
        }
        i++;
    }
}

const char* Event2::GetString(const char* key){
    unsigned int bytePos = map[key];
    return (char*)&(bytes[bytePos]);
}

void Event2::SetBytes(const uint8_t* bytes, unsigned int numBytes){
    this->bytes = std::vector<uint8_t>(bytes, bytes+numBytes);
}


}