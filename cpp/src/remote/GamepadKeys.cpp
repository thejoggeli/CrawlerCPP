#include "GamepadKeys.h"

namespace Crawler {

std::string GamepadKeyToString(GamepadKey key){
	switch(key){
	case GamepadKey::A: return "A";
	case GamepadKey::B: return "B";
	case GamepadKey::X: return "X";
	case GamepadKey::Y: return "Y";
	case GamepadKey::Up: return "Up";
	case GamepadKey::Down: return "Down";
	case GamepadKey::Left: return "Left";
	case GamepadKey::Right: return "Right";
	case GamepadKey::Start: return "Start";
	case GamepadKey::Select: return "Select";
	case GamepadKey::LeftJoystick: return "LeftJoystick";
	case GamepadKey::RightJoystick: return "RightJoystick";
	default: return "KeyCode(" + std::to_string(static_cast<int>(key)) + ")";
	}
}
std::string GamepadKeyStateToString(GamepadKeyState state){
	switch(state){
	case GamepadKeyState::Pressed: return "Pressed";
	case GamepadKeyState::Released: return "Released";
	case GamepadKeyState::JoystickMove: return "JoystickMove";
	default: return "KeyState(" + std::to_string(static_cast<int>(state)) + ")";
	}
}

}
