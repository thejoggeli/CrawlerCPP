#include "WeightSensor.h"
#include "libi2c/i2c.h"
#include "MuxI2C.h"
#include "NAU7802/NAU7802.h"
#include "core/Log.h"

namespace Crawler {

WeightSensor::WeightSensor(){
    nau = new NAU7802();
}

WeightSensor::~WeightSensor(){
    
}

bool WeightSensor::Init(MuxI2C* mux, int channel){

    if(i2c){
        return false;
    }

    this->mux = mux;
    this->channel = channel;

    i2c = new I2CDevice();
    i2c_init_device(i2c);
    i2c->bus = mux->GetBus();
    i2c->addr = 0x2A;

    if(!nau->begin(i2c, true)){
        LogError("WeightSensor", iLog << "NAU7802 begin failed");
        return false;
    }

    return true;
}

int32_t WeightSensor::GetValue(){
    mux->OpenChannel(channel);
    int32_t val = nau->getReading();
    mux->CloseChannel(channel);
    return val;
}

}