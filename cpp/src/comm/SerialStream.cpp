#include "SerialStream.h"

SerialStream::SerialStream(){
    std::lock_guard<std::mutex> lock(mtx);
}

int SerialStream::open(const char* device, uint32_t baud){
    std::lock_guard<std::mutex> lock(mtx);
    return serialib.openDevice(device, baud);
}

void SerialStream::close(){
    std::lock_guard<std::mutex> lock(mtx);
    serialib.closeDevice();
}


int SerialStream::available(){
    std::lock_guard<std::mutex> lock(mtx);
    return serialib.available();
}

int SerialStream::readBytes(uint8_t* buffer, size_t length, unsigned int timeout){
    std::lock_guard<std::mutex> lock(mtx);
    return serialib.readBytes(buffer, length, timeout);
}

int SerialStream::read(unsigned int timeout){
    std::lock_guard<std::mutex> lock(mtx);
    char c;
    return serialib.readChar(&c, timeout);
}

int SerialStream::flushRead(){
    std::lock_guard<std::mutex> lock(mtx);
    return serialib.flushReceiver();
}

int  SerialStream::write(const uint8_t value){
    std::lock_guard<std::mutex> lock(mtx);
    return serialib.writeChar(value);
}

int SerialStream::write(const uint8_t* buffer, size_t size){
    std::lock_guard<std::mutex> lock(mtx);
    return serialib.writeBytes(buffer, size);
}

