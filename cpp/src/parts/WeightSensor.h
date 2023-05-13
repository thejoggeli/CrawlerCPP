#pragma once

class I2CDevice;
class HX711;

namespace Crawler {

class WeightSensor {

    HX711* hx711;

public:


    WeightSensor();
    ~WeightSensor();

    bool Init(unsigned int dataPin, unsigned int clockPin);
    float GetValue();

};

}