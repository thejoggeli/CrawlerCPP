#pragma once

#include <cstdint>

class I2CDevice;
class NAU7802;

namespace Crawler {

class MuxI2C;

class WeightSensor {

private: 

    I2CDevice* i2c = nullptr;
    MuxI2C* mux = nullptr;
    NAU7802* nau = nullptr;
    int channel;

public:


    WeightSensor();
    ~WeightSensor();

    bool Init(MuxI2C* mux, int channel);
    int32_t GetValue();

};

}