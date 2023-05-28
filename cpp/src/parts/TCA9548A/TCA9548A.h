/*   Version: 1.1.3 |  License: Apache 2.0  |  Author: JDWifWaf@gmail.com   */

#pragma once

#include <cstdint>

class I2CDevice;

namespace Crawler {

/* Channel hex values for writeRegister() function */
#define TCA_CHANNEL_0 0x1
#define TCA_CHANNEL_1 0x2
#define TCA_CHANNEL_2 0x4
#define TCA_CHANNEL_3 0x8
#define TCA_CHANNEL_4 0x10
#define TCA_CHANNEL_5 0x20
#define TCA_CHANNEL_6 0x40
#define TCA_CHANNEL_7 0x80

class TCA9548A
{
    public:
        TCA9548A(uint8_t address = 0x70);  // Default IC Address

        void begin(I2CDevice* dev);
        void openChannel(uint8_t channel);
        void closeChannel(uint8_t channel);
        void writeRegister(uint8_t value);
        inline uint8_t readRegister() { return (uint8_t)read(); }
        void closeAll();
        void openAll();
        
    protected:
    private:
        I2CDevice *i2c = nullptr;
        uint8_t _address;
        uint8_t _channels;

        void write(uint8_t inData);
        uint8_t read();
};

}