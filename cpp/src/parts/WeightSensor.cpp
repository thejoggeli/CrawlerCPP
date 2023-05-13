#include "WeightSensor.h"
#include "libi2c/i2c.h"
#include "parts/HX711/HX711.h"

namespace Crawler {

WeightSensor::WeightSensor(){
    this->hx711 = new HX711();
}

WeightSensor::~WeightSensor(){
    if(this->hx711){
        delete this->hx711;
    }
}

bool WeightSensor::Init(unsigned int dataPin, unsigned int clockPin){
    this->hx711->begin(dataPin, clockPin);
    this->hx711->set_average_mode();
    return true;
}

float WeightSensor::GetValue(){
    return hx711->get_value(5);
}

}