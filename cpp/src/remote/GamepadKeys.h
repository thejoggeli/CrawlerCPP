#pragma once

#include <string>
#include <cstdint>
#include <memory>


#define NUM_GAMEPAD_KEYS 0xFF
#define NUM_GAMEPAD_JOYSTICKS 2

namespace Crawler {

enum class GamepadKey : uint8_t {
        None = 0x00,
        A = 0x10, 
        B = 0x11, 
        X = 0x12, 
        Y = 0x13,
        Up = 0x20,  
        Down = 0x21, 
        Left = 0x22, 
        Right = 0x23,
        Start = 0x40, 
        Select = 0x41,
        LeftJoystick = 0xF0, 
        RightJoystick = 0xF1,
};

enum class GamepadKeyState : uint8_t {
        Pressed = 0,
        Released = 1,
};

std::shared_ptr<std::string> GamepadKeyToString(GamepadKey key);
std::shared_ptr<std::string> GamepadKeyToString(uint8_t key);
std::shared_ptr<std::string> GamepadKeyStateToString(GamepadKeyState state);
std::shared_ptr<std::string> GamepadKeyStateToString(uint8_t key);

}
