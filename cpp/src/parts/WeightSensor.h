#pragma once

class I2CDevice;

namespace Crawler {

class MuxI2C;

class WeightSensor {

private: 

    I2CDevice* i2c = nullptr;
    MuxI2C* mux = nullptr;
    int channel;

public:


    WeightSensor();
    ~WeightSensor();

    bool Init(MuxI2C* mux, int channel);
    float GetValue();

};

}