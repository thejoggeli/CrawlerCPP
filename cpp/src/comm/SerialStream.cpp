#include "SerialStream.h"

SerialStream::SerialStream(){
    
}

int SerialStream::open(const char* device, uint32_t baud){
    return serialib.openDevice(device, baud);
}

void SerialStream::close(){
    serialib.closeDevice();
}


int SerialStream::available(){
    return serialib.available();
}

int SerialStream::readBytes(uint8_t* buffer, size_t length, unsigned int timeout){
    return serialib.readBytes(buffer, length, timeout);
}

int SerialStream::read(unsigned int timeout){
    char c;
    return serialib.readChar(&c, timeout);
}

int SerialStream::flushRead(){
    return serialib.flushReceiver();
}

int  SerialStream::write(const uint8_t value){
    return serialib.writeChar(value);
}

int SerialStream::write(const uint8_t* buffer, size_t size){
    return serialib.writeBytes(buffer, size);
}

