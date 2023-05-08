#pragma once

namespace Crawler {

class MonoLED {
public:

    unsigned int pin;
    bool state;

    MonoLED(unsigned int pin);
    void Init(bool initialState = 0);
    void ToggleState();
    void SetState(bool state);
    void Cleanup();

};

}