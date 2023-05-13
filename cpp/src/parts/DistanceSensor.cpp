#include "DistanceSensor.h"
#include "VCNL4010/VCNL4010.h"

namespace Crawler {

DistanceSensor::DistanceSensor(){
    this->vcnl = new VCNL4010();
}

DistanceSensor::~DistanceSensor(){
    if(this->vcnl){
        delete this->vcnl;
    }
}

bool DistanceSensor::Init(I2CDevice* i2cDevice){
    if (initialized || !vcnl->begin(i2cDevice)){
        return false;
    }
    initialized = true;
    return true;
}

uint16_t DistanceSensor::ReadAmbient(){
    return vcnl->readAmbient();
}

uint16_t DistanceSensor::ReadProximity(){
    return vcnl->readProximity();
}

}