#include "DistanceSensor.h"
#include "VCNL4010/VCNL4010.h"
#include "libi2c/i2c.h"
#include "MuxI2C.h"

namespace Crawler {

DistanceSensor::DistanceSensor(){
    this->vcnl = new VCNL4010();
}

DistanceSensor::~DistanceSensor(){
    if(this->vcnl){
        delete this->vcnl;
    }
    if(this->i2c){
        delete this->i2c;
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