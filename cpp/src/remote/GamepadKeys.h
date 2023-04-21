#pragma once

#include <string>
#include <cstdint>



#define NUM_GAMEPAD_KEYS 0xFF
#define NUM_GAMEPAD_JOYSTICKS 2
#define GAMEPAD_JOYSTICK_MASK 0xF0

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
		JoystickMove = 2,
};

std::string KeyCodeToString(GamepadKey key);
std::string GamepadKeyStateToString(GamepadKeyState state);

}
