#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"
#include "GamepadKeys.h"
#include <vector>
#include <memory>

namespace Crawler {

class Event;

class Client {
public:
	int id;
	uint32_t flags = 0;
	bool connected;
	std::vector<std::shared_ptr<Event>> events;
	std::vector<std::shared_ptr<Event>> newEvents;

	Client(int id);
	void Update();
	bool IsKeyDown(KeyCode code);
	bool OnKeyDown(KeyCode code);
	bool OnKeyUp(KeyCode code);
	Eigen::Vector2f GetJoystickPosition(KeyCode code);

	void AddMessage(const std::string& str);
	void PopNewEvents();

	void SetFlag(uint32_t flag);
	void UnsetFlag(uint32_t flag);
	bool IsFlagSet(uint32_t flag);

private:
	bool isDownMap[static_cast<int>(KeyCode::KEY_CODE_MAX)];
	bool onDownMap[static_cast<int>(KeyCode::KEY_CODE_MAX)];
	bool onUpMap[static_cast<int>(KeyCode::KEY_CODE_MAX)];
	Eigen::Vector2f joystickPositions[2];
};

}


