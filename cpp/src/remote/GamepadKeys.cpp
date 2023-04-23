#include "GamepadKeys.h"

namespace Crawler {

std::shared_ptr<std::string> GamepadKeyToString(uint8_t key){
	return GamepadKeyToString((GamepadKey)key);
}

std::shared_ptr<std::string> GamepadKeyToString(GamepadKey key){
	switch(key){
	case GamepadKey::A: return std::make_shared<std::string>("A");
	case GamepadKey::B: return std::make_shared<std::string>("B");
	case GamepadKey::X: return std::make_shared<std::string>("X");
	case GamepadKey::Y: return std::make_shared<std::string>("Y");
	case GamepadKey::Up: return std::make_shared<std::string>("Up");
	case GamepadKey::Down: return std::make_shared<std::string>("Down");
	case GamepadKey::Left: return std::make_shared<std::string>("Left");
	case GamepadKey::Right: return std::make_shared<std::string>("Right");
	case GamepadKey::Start: return std::make_shared<std::string>("Start");
	case GamepadKey::Select: return std::make_shared<std::string>("Select");
	case GamepadKey::LeftJoystick: return std::make_shared<std::string>("LeftJoystick");
	case GamepadKey::RightJoystick: return std::make_shared<std::string>("RightJoystick");
	default: return std::make_shared<std::string>("KeyCode(" + std::to_string(static_cast<int>(key)) + ")");
	}
}

std::shared_ptr<std::string> GamepadKeyStateToString(uint8_t key){
	return GamepadKeyStateToString((GamepadKeyState)key);
}

std::shared_ptr<std::string> GamepadKeyStateToString(GamepadKeyState state){
	switch(state){
	case GamepadKeyState::Pressed: return std::make_shared<std::string>("Pressed");
	case GamepadKeyState::Released: return std::make_shared<std::string>("Released");
	default: return std::make_shared<std::string>("KeyState(" + std::to_string(static_cast<int>(state)) + ")");
	}
}

}
