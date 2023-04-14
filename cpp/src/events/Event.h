#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"
#include "remote/GamepadKeys.h"
#include <string>
#include <vector>

namespace Crawler {

enum class EventType {
	Message = 0,
	Input = 10,
	Joystick = 15,
};

class Event {
public:
    bool fromClient = false;
    int clientId = -1;
    EventType type;
    Event(EventType type);
	virtual ~Event();
    virtual std::string ToString();
    std::string GetToStringPart();
	static std::string EventTypeToString(EventType);
};

class MessageEvent : public Event {
private:
	std::vector<std::string> params;
public:
	std::string message;
	int numParams;
	MessageEvent(const std::string& message);
	void AddParam(const std::string& param);
	std::string GetParam(int index);
	const std::string& GetParamConstRef(int index);
	int GetParamInt(int index);
	bool GetParamBool(int index);
	float GetParamFloat(int index);
    double GetParamDouble(int index);
	std::string ToString();
};

class InputEvent : public Event {
public:
	KeyCode code;
    KeyState state;
	InputEvent(KeyCode code, KeyState state);
	virtual std::string ToString();
};

class JoystickEvent : public Event {
public:
	KeyCode code;
	KeyState state;
	Eigen::Vector2f position;
	JoystickEvent(KeyCode code, KeyState state);
	std::string ToString();
};

}


