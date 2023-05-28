#pragma once

#include <cstdint>

class I2CDevice;

namespace Crawler {

class VCNL4010;
class MuxI2C;

class DistanceSensor {
private:

    VCNL4010* vcnl = nullptr;
    I2CDevice* i2c = nullptr;
    MuxI2C* mux = nullptr;
    int channel;

public:

    bool initialized = false; 

    DistanceSensor();
    ~DistanceSensor();

    bool Init(MuxI2C* mux, int channel);
    uint16_t ReadProximity(); 
    uint16_t ReadAmbient();

};

}