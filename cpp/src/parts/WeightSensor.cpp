#include "WeightSensor.h"
#include "libi2c/i2c.h"
#include "MuxI2C.h"

namespace Crawler {

WeightSensor::WeightSensor(){

}

WeightSensor::~WeightSensor(){
    
}

bool WeightSensor::Init(MuxI2C* mux, int channel){
    return true;
}

float WeightSensor::GetValue(){
    mux->OpenChannel(channel);
    return 0.0f;
    mux->CloseChannel(channel);
}

}