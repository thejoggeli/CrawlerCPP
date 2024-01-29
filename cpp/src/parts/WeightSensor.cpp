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
    if(nau){
        if(mux){
            mux->OpenChannel(channel);
            nau->powerDown();
            mux->CloseChannel(channel);
        }
        delete nau;
        nau = nullptr;
    }
    if(i2c){
        delete i2c;
        i2c = nullptr;
    }
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

    mux->OpenChannel(channel);
    if(!nau->begin(i2c, true)){
        mux->CloseChannel(channel);
        LogError("WeightSensor", iLog << "NAU7802 begin failed");
        return false;
    }
    mux->CloseChannel(channel);

    return true;
}

int32_t WeightSensor::GetValue(){
    mux->OpenChannel(channel);
    int32_t val = nau->getReading();
    mux->CloseChannel(channel);
    return val;
}

}