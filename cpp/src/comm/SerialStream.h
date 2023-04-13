#pragma once

#include "serialib/Serialib.h"

class SerialStream {
   private:
    Serialib serialib;

   public:
    SerialStream();

    int open(const char* device, uint32_t baud);
    void close();
    
    int available();

    int readBytes(uint8_t* buffer, size_t length, unsigned int timeout);
    int read(unsigned int timeout);
    int flushRead();
    
    int write(const uint8_t value);
    int write(const uint8_t* buffer, size_t size);

};