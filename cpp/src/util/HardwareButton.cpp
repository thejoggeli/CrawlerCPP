#include "HardwareButton.h"
#include "core/Time.h"
#include <JetsonGPIO.h>

using namespace std;
using namespace Crawler;

HardwareButton::HardwareButton(unsigned int pin, uint64_t debounceTimeMicros){
    this->pin = pin;
    this->debounceTimeMicros = debounceTimeMicros;
}

void HardwareButton::Init(){
    GPIO::setup(pin, GPIO::IN);
    GPIO::add_event_detect(pin, GPIO::FALLING, nullptr, debounceTimeMicros/1000);
}

void HardwareButton::Update(){ 
    bool down;
    if(GPIO::event_detected(pin)){
        down = true;
    } else {
        down = GPIO::input(pin) ? false : true;
    }
    onPress = 0;
    onRelease = 0;
    if(down && !isPressed && (Time::currentTimeMicros-lastChangeTimeMicros > debounceTimeMicros)){
        lastChangeTimeMicros = Time::currentTimeMicros;
        onPress = 1;
        isPressed = 1;
    } else if(!down && isPressed && (Time::currentTimeMicros-lastChangeTimeMicros > debounceTimeMicros)){
        lastChangeTimeMicros = Time::currentTimeMicros;
        onRelease = 1;
        isPressed = 0;
    }
}

void HardwareButton::Cleanup(){
    GPIO::cleanup(pin);
}


