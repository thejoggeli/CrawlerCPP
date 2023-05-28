/*   Version: 1.1.3  |  License: Apache 2.0  |  Author: JDWifWaf@gmail.com   */
#include "TCA9548A.h"
#include "libi2c/i2c.h"

namespace Crawler {

TCA9548A::TCA9548A(uint8_t address) : _address(address) {}

void TCA9548A::begin(I2CDevice* dev){
    this->i2c = dev;
}

void TCA9548A::openChannel(uint8_t channel)
{
    uint8_t buff = 0x00;    
    buff = 1 << channel;
    this->_channels |= buff;
    write(this->_channels);
}

void TCA9548A::closeChannel(uint8_t channel)
{
    uint8_t buff = 0x00;    
    buff = 1 << channel;    
    this->_channels ^= buff;

    write(this->_channels);
}

void TCA9548A::closeAll()
{
    this->_channels = 0x00;
    write(this->_channels);
}

void TCA9548A::openAll()
{
    this->_channels = 0xFF;
    write(this->_channels);
}

void TCA9548A::writeRegister(uint8_t value)
{
    this->_channels = value;
    write(this->_channels);
}

void TCA9548A::write(uint8_t inData)
{
    i2c_write(i2c, 0, &inData, 1);
}

uint8_t TCA9548A::read()
{
    uint8_t buf;
    i2c_read(i2c, 0, &buf, 1);
    return buf;
}

}