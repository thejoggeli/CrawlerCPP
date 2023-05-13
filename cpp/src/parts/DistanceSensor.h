#pragma once

#include <cstdint>

class I2CDevice;

namespace Crawler {

class VCNL4010;


class DistanceSensor {
private:

    VCNL4010* vcnl;

public:

    bool initialized = false; 

    DistanceSensor();
    ~DistanceSensor();

    bool Init(I2CDevice* i2cDevice);
    uint16_t ReadProximity(); 
    uint16_t ReadAmbient();

};

}