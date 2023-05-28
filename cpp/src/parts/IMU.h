#pragma once

#include <cstdint>

class I2CDevice;

namespace Crawler {

class IMU {

private: 

    I2CDevice* i2c = nullptr;
    void* ctx = nullptr;

    friend int32_t platform_write(void *handle, uint8_t reg, const uint8_t *buf, uint16_t len);
    friend int32_t platform_read(void *handle, uint8_t reg, uint8_t *buf, uint16_t len);

public:

    IMU();
    ~IMU();

    bool Init(unsigned int bus);

    void ReadAcceleration(float values[3]);
    void ReadGyro(float values[3]);
    float ReadTemperature();

};

}