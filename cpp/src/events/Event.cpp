#include "Event.h"
#include <string>

namespace Crawler {

Event::Event(EventType type){
	this->type = type;
}
Event::~Event(){}
std::string Event::ToString(){
	return GetToStringPart();
}
std::string Event::GetToStringPart(){
	if(fromClient){
		return EventTypeToString(type) + " (id=" + std::to_string(clientId) + ")";
	} else {
		return EventTypeToString(type) + " (System)";
	}
}
std::string Event::EventTypeToString(EventType type){
	switch(type){
		case EventType::Input: return "Input";
		case EventType::Message: return "Message";
		case EventType::Joystick: return "Joystick";
	}
	return "Event";
}

MessageEvent::MessageEvent(const std::string& message) : Event(EventType::Message){
	this->message = message;
}
void MessageEvent::AddParam(const std::string &param){
	params.push_back(param);
	numParams = params.size();
}
std::string MessageEvent::GetParam(int index){
	if(index >= params.size()) return "";
	return params[index];
}
const std::string& MessageEvent::GetParamConstRef(int index){
	return params[index];
}
int MessageEvent::GetParamInt(int index){
	if(index >= params.size()) return 0;
	return std::stoi(params[index]);

}
bool MessageEvent::GetParamBool(int index){
	if(index >= params.size()) return 0;
    return params[index] == "0" ? false : true;

}
float MessageEvent::GetParamFloat(int index){
	if(index >= params.size()) return 0;
	return std::stof(params[index]);
}
double MessageEvent::GetParamDouble(int index){
    if(index >= params.size()) return 0;
    return std::stod(params[index]);
}

std::string MessageEvent::ToString(){
	std::string str = GetToStringPart() + " / " + message;
	for(auto const& param: params){
		str += " / " + param;
	}
	return str;
}

InputEvent::InputEvent(KeyCode code, KeyState state) : Event(EventType::Input){
	this->code = code;
	this->state = state;
}
std::string InputEvent::ToString(){
	return GetToStringPart()
			+ " / " + KeyCodeToString(code)
			+ " / " + KeyStateToString(state);
}

JoystickEvent::JoystickEvent(KeyCode code, KeyState state) : Event(EventType::Joystick){
	this->code = code;
	this->state = state;
}
std::string JoystickEvent::ToString(){
	return GetToStringPart()
			+ " / " + KeyCodeToString(code)
			+ " / " + KeyStateToString(state)
			+ " / x: " + std::to_string(this->position[0])
			+ " / y: " + std::to_string(this->position[1]);
}

}
