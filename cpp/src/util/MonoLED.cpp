#include "MonoLED.h"
#include <JetsonGPIO.h>
#include "core/Log.h"

using namespace Crawler;

MonoLED::MonoLED(unsigned int pin){
    this->pin = pin;
}

void MonoLED::Init(bool initialState){
    LogInfo("MonoLED", iLog << "init, pin=" << pin << ", initialState=" << initialState);
    GPIO::setup(pin, GPIO::OUT);
    SetState(initialState);
}

void MonoLED::ToggleState(){
    state = !state;
    GPIO::output(pin, state);
}

void MonoLED::SetState(bool newState){
    if(this->state == newState){
        return;
    }
    this->state = newState;
    GPIO::output(pin, this->state);
}

void MonoLED::Cleanup(){
    SetState(false);
    LogInfo("MonoLED", iLog << "cleanup, pin=" << pin);
    GPIO::cleanup(pin);
}



