#pragma once

#include "libi2c/i2c.h"
#include <cstdint>

class I2CDevice;

namespace Crawler {

enum VCNL4010Mode {
    Invalid, Ambient, Proximity
};

class VCNL4010 {
private:

    uint8_t address;
    VCNL4010Mode mode = VCNL4010Mode::Invalid;
    I2CDevice *i2c = nullptr; 

public:
     
    VCNL4010();
    bool begin(I2CDevice* i2cDevice);
    uint16_t readProximity(void);
    uint16_t readAmbient(void); 
    void setMode(VCNL4010Mode mode);

private:

    void write8(uint8_t address, uint8_t data);
    uint16_t read16(uint8_t address);
    uint8_t read8(uint8_t address);

};

}