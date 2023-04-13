#pragma once

#include <cstdint>

class HardwareButton {
public:

    bool onPress = false;
    bool onRelease = false;
    bool isPressed = false;

    unsigned int pin = 0;
    uint64_t debounceTimeMicros = 250; 
    uint64_t lastChangeTimeMicros = 0;

    HardwareButton(unsigned int pin, uint64_t debounceTimeMicros);
    void Init();
    void Update();
    void Cleanup();

};
