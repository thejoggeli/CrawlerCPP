#pragma once

class I2CDevice;

namespace Crawler {

class IMU {

private: 

    I2CDevice* i2c = nullptr;

public:

    IMU();
    ~IMU();

    bool Init(unsigned int bus);

};

}