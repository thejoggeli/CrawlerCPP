#pragma once

#include <cstdint>

class I2CDevice;

namespace Crawler {

class TCA9548A;

class MuxI2C {

private:

    TCA9548A* tca = nullptr;
    I2CDevice* i2c = nullptr;

public: 

    MuxI2C();
    ~MuxI2C();

    bool Init(unsigned int bus);
    void Shutdown();

    void OpenChannel(uint8_t channel);
    void CloseChannel(uint8_t channel);

    void CloseAll();
    void OpenAll();

    int GetBus();

};

}