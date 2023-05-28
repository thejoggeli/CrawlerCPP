#include "IMU.h"
#include "libi2c/i2c.h"

namespace Crawler {

IMU::IMU(){
    
}

IMU::~IMU(){

}

bool IMU::Init(unsigned int bus){
    
    if(i2c){
        return false;
    }

    i2c = new I2CDevice();
    i2c->bus = bus;
    i2c->addr = 0x13;
    i2c->iaddr_bytes = 1;
    i2c->page_bytes = 16;
    i2c->tenbit = false;

    return true;
}


}