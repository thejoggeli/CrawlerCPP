#include "VCNL4010.h"
#include "core/Log.h"
#include "core/Time.h"

namespace Crawler {

VCNL4010::VCNL4010() {

}

bool VCNL4010::begin(I2CDevice* i2c) {

    this->i2c = i2c;

    uint8_t rev = read8(0x81); // read Product ID Revision Register
    LogDebug("VCNL4010", iLog << "read(0x81) returned " << (int)rev);
    if ((rev & 0xF0) != 0x20) {
        LogError("VCNL4010", "(rev & 0xF0) != 0x20");
        return false;
    }

    write8(0x80, 0x00); // disable periodic measurements
    
    // set proximity rate frequency
    // values in Hz (default = 0)
    // 0 =  1.95, 1 =  3.9, 2 =   7.8, 3 =  16.6
    // 4 = 31.25, 5 = 62.5, 6 = 125.0, 7 = 250.0
    write8(0x82, 7); 

    // set led current 
    // the current will be (value*10) mA
    // e.g. value = 10 corresponds to 100mA
    // max current is 200mA
    write8(0x83, 20); 

    // 0=off, 1=on
    unsigned int continuousConversionMode = 1; // default = 0

    // 0 = 1 samples/s, 1 = 2 samples/s, 2 = 3 samples/s, 3 =  4 samples/s
    // 4 = 5 samples/s, 5 = 6 samples/s, 6 = 8 samples/s, 7 = 10 samples/s
    unsigned int ambientLightMeasurementRate = 7; // default = 1

    // 0=off, 1=on
    unsigned int autoOffsetCompensation = 1; // default = 1

    // 0 =  1 sample,  1 =  2 samples, 2 =  4 samples, 3 =   8 samples
    // 4 = 16 samples, 5 = 32 samples, 6 = 64 samples, 7 = 128 samples
    unsigned int averagingFunction = 2; // default = 5

    uint8_t reg84 = 0;
    reg84 |= continuousConversionMode << 7;
    reg84 |= ambientLightMeasurementRate << 4;
    reg84 |= autoOffsetCompensation << 3;
    reg84 |= averagingFunction;
    write8(0x84, reg84); // set Ambient Light Parameter Register

    write8(0x89, 0x00); // disable interrupts
    setMode(VCNL4010Mode::Proximity);

    return true;
}

void VCNL4010::setMode(VCNL4010Mode mode){
    if(mode == VCNL4010Mode::Proximity){
        write8(0x80, 0x03); // enable periodic proximity measurements
    } else if(mode == VCNL4010Mode::Ambient){
        write8(0x80, 0x05); // enable periodic ambient light measurements
    }
    this->mode = mode;
}

uint16_t VCNL4010::readProximity(void) {
    if(mode != VCNL4010Mode::Proximity || !i2c){
        return 0;
    }
    while (1) {
        uint8_t result = read8(0x80);
        // LogDebug("VCNL4010", iLog << "result=" << (int)result);
        if (result & 0x20) {
            return read16(0x87);
        }
        Time::SleepMicros(100);
    }
}

uint16_t VCNL4010::readAmbient(void) {
    if(mode != VCNL4010Mode::Ambient || !i2c){
        return 0;
    }
    while (1) {
        uint8_t result = read8(0x80);
        // LogDebug("VCNL4010", iLog << "result=" << (int)result);
        if (result & 0x40) {
            return read16(0x85);
        }
        Time::SleepMicros(100);
    }
}

// Read 1 byte from the VCNL4000 at 'address'
uint8_t VCNL4010::read8(uint8_t address) {
    uint8_t buffer[1] = {address};
    // i2c_write(i2c, address, buffer, 1);
    i2c_read(i2c, address, buffer, 1);
    return buffer[0];
}

// Read 2 byte from the VCNL4000 at 'address'
uint16_t VCNL4010::read16(uint8_t address) {
    uint8_t buffer[2] = {address, 0};
    // i2c_write(i2c, address, buffer, 1);
    i2c_read(i2c, address, buffer, 2);
    return (uint16_t(buffer[0]) << 8) | uint16_t(buffer[1]);
}

// write 1 byte
void VCNL4010::write8(uint8_t address, uint8_t data) {
    // uint8_t buffer[2] = {address, data};
    // i2c_dev->write(buffer, 2);
    uint8_t buffer[1] = {data};
    i2c_write(i2c, address, buffer, 1);
}

}