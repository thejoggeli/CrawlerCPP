#include "DistanceSensor.h"
#include "VCNL4010/VCNL4010.h"
#include "libi2c/i2c.h"
#include "MuxI2C.h"

namespace Crawler {

DistanceSensor::DistanceSensor(){
    vcnl = new VCNL4010();
}

DistanceSensor::~DistanceSensor(){
    if(vcnl){
        delete vcnl;
        vcnl = nullptr;
    }
    if(i2c){
        delete i2c;
        i2c = nullptr;
    }
}

bool DistanceSensor::Init(MuxI2C* mux, int channel){
    
    if(initialized || i2c){
        return false;
    }

    this->mux = mux;
    this->channel = channel;

    i2c = new I2CDevice();
    i2c_init_device(i2c);
    i2c->bus = mux->GetBus();
    i2c->addr = 0x13;

    mux->OpenChannel(channel);
    if (!vcnl->begin(i2c)){
        mux->CloseChannel(channel);
        delete i2c;
        return false;
    }
    mux->CloseChannel(channel);

    initialized = true;
    return true;
}

uint16_t DistanceSensor::ReadAmbient(){
    mux->OpenChannel(channel);
    uint16_t val = vcnl->readAmbient();
    mux->CloseChannel(channel);
    return val;
}

uint16_t DistanceSensor::ReadProximity(){
    mux->OpenChannel(channel);
    uint16_t val = vcnl->readProximity();
    mux->CloseChannel(channel);
    return val;
}

}