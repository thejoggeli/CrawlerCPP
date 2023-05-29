#include "MuxI2C.h"
#include "TCA9548A/TCA9548A.h"
#include "core/Log.h"
#include "libi2c/i2c.h"

namespace Crawler {

MuxI2C::MuxI2C(){
    tca = new TCA9548A();
}

MuxI2C::~MuxI2C(){
    if(tca){
        tca->closeAll();
        delete tca;
        tca = nullptr;
    }
    if(i2c){
        delete i2c;
        i2c = nullptr;
    }

}

bool MuxI2C::Init(unsigned int bus){

    if(i2c){
        return false;
    }

    i2c = new I2CDevice();
    i2c_init_device(i2c);
    i2c->bus = bus;
    i2c->addr = 0x70;

    tca->begin(i2c);
    tca->closeAll();

    return true;
}

void MuxI2C::Shutdown(){
    if(tca){
        tca->closeAll();
    }
}

void MuxI2C::OpenChannel(uint8_t channel){
    tca->openChannel(channel);
}
void MuxI2C::CloseChannel(uint8_t channel){
    tca->closeChannel(channel);
}

void MuxI2C::CloseAll(){
    tca->openAll();
}
void MuxI2C::OpenAll(){
    tca->closeAll();
}

int MuxI2C::GetBus(){
    if(i2c){
        return i2c->bus;
    }
    return -1;
}

}