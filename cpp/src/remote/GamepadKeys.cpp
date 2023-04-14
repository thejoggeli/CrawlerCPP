#include "GamepadKeys.h"

namespace Crawler {

std::string KeyCodeToString(KeyCode KeyCode){
	switch(KeyCode){
	case KeyCode::A: return "A";
	case KeyCode::B: return "B";
	case KeyCode::X: return "X";
	case KeyCode::Y: return "Y";
	case KeyCode::Up: return "Up";
	case KeyCode::Down: return "Down";
	case KeyCode::Left: return "Left";
	case KeyCode::Right: return "Right";
	case KeyCode::Start: return "Start";
	case KeyCode::Select: return "Select";
	case KeyCode::LeftJoystick: return "LeftJoystick";
	case KeyCode::RightJoystick: return "RightJoystick";
	default: return "KeyCode(" + std::to_string(static_cast<int>(KeyCode)) + ")";
	}
}
std::string KeyStateToString(KeyState keyState){
	switch(keyState){
	case KeyState::Pressed: return "Pressed";
	case KeyState::Released: return "Released";
	case KeyState::JoystickMove: return "JoystickMove";
	default: return "KeyState(" + std::to_string(static_cast<int>(keyState)) + ")";
	}
}

}
