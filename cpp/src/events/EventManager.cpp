#include "EventManager.h"
#include "Event.h"
#include "core/Log.h"
#include "core/Config.h"
#include <algorithm>
#include <unordered_map>

using namespace std;

namespace Crawler {

static bool logEvents = false;

int EventManager::initCounter = 0;
unsigned int EventManager::eventCounterTemp = 0;
unsigned int EventManager::eventCounterTotal = 0;

static vector<shared_ptr<Event>> events;
static vector<shared_ptr<Event>> newEvents;
static unordered_map<string, EventType> KeyCode;

static std::unordered_map<std::string, std::vector<std::pair<void*, void (*)(void*, MessageEvent&)>>> messageSubscriptions;


bool EventManager::Init(){
	if(++initCounter > 1) return false;
	Log(LOG_INFO, "EventManager", "Initializing");
	KeyCode.insert({"msg", EventType::Message});
	KeyCode.insert({"inp", EventType::Input});
	KeyCode.insert({"joy", EventType::Joystick});
	logEvents = Config::GetBool("log_info_events", true);
	return true;
}

void EventManager::AddEvent(const shared_ptr<Event>& event){
	if(logEvents){
		Log(LOG_INFO, "EventManager", event->ToString());
	}
	eventCounterTemp++;
	eventCounterTotal++;
	newEvents.push_back(event);
}

void EventManager::PopNewEvents(){
	events.clear();
	events.insert(events.end(), newEvents.begin(), newEvents.end());
	newEvents.clear();
}

void EventManager::Update(){
	for(auto& event: events){
		if(event->type == EventType::Message){
			MessageEvent& msg = (MessageEvent&)(*event);
			auto search = messageSubscriptions.find(msg.message);
			if (search != messageSubscriptions.end()){
				auto& vector = search->second;
				for(auto& handler: vector){
					handler.second(handler.first, msg);
				}
			}
		}
	}
}
shared_ptr<Event> EventManager::ParseMessage(const string& str){
	shared_ptr<Event> event = nullptr;
	// check if string length is at least 3
	int strlen = str.length();
	if(strlen < 3){
		Log(LOG_ERROR, "EventManager", "Invalid message: " + str);
		return nullptr;
	}
	// get event type
	string sType = str.substr(0,3);
	auto search = KeyCode.find(sType);
	if(search == KeyCode.end()){
		Log(LOG_ERROR, "EventManager", "Invalid event type: " + sType);
		return nullptr;
	}
	EventType eventType = search->second;
	// create event
	switch(eventType){
	case EventType::Message: {
		// get command
		unsigned long pos = str.find(":");
		string message = str.substr(3, pos-3);
		shared_ptr<MessageEvent> messageEvent = make_shared<MessageEvent>(message);
		// get parameters
		string remaining = str.substr(pos+1);
		pos = remaining.find("/");
		while(pos < remaining.size()){
			unsigned long length = stoul(remaining.substr(0, pos));
			string param = remaining.substr(pos+1, length);
			remaining = remaining.substr(pos+1+length);
			pos = remaining.find("/");
			messageEvent->AddParam(param);
		}
		event = messageEvent;
		break;
	}
	case EventType::Input: {
		// input event
		// [0-2] = "inp"
		// [3] 0=pressed, 1=released
		// [4+] keycode
		if(strlen < 5){
			Log(LOG_ERROR, "EventManager", "Input event too short");
			return nullptr;
		}
		int state = stoi(str.substr(3,1));
		int code = stoi(str.substr(4));
		if(state < 0 || state > 1){
			Log(LOG_ERROR, "EventManager", "Invalid input key state value");
			return nullptr;
		}
		if(state < 0 || code > static_cast<int>(KeyCode::KEY_CODE_MAX)){
			Log(LOG_ERROR, "EventManager", "Invalid input key code value");
			return nullptr;
		}
		event = make_shared<InputEvent>(static_cast<enum KeyCode>(code), static_cast<KeyState>(state));
		break;
	}
	case EventType::Joystick: {
		// input event
		// [0-2] = "joy"
		// [3] KeyState 0=Pressed, 1=Released, 2=JoystickMove
		// [4] KeyCode 0=LeftJoystick, 1=RightJoystick
		// [5] Delimiter
		// [6-14] x position
		// [15] Delimiter
		// [16-24] y position
		int state = stoi(str.substr(3,1));
		int code = stoi(str.substr(4,1));
		enum KeyCode keyCode = (code == 0) ? KeyCode::LeftJoystick : KeyCode::RightJoystick;
		shared_ptr<JoystickEvent> jev = make_shared<JoystickEvent>(keyCode, static_cast<KeyState>(state));
		if(state == 2){
			float x = stof(str.substr(6,9));
			float y = stof(str.substr(16,9));
			jev->position = Eigen::Vector2f(x, y);
		} else {
			jev->position = Eigen::Vector2f(0.0f, 0.0f);
		}
		event = jev;
	}
	}
	if(event == nullptr){
		Log(LOG_ERROR, "EventManager", "Parser couldn't handle event type: " + sType);
	}
	return event;
}

void EventManager::SubscribeMessage(const std::string &message, void* obj, void (*handler)(void*, MessageEvent&)){
	std::make_pair<int, int>(5,5);
	std::pair<void*, void (*)(void*, MessageEvent&)> pair;
	pair = std::make_pair(obj, handler);
	messageSubscriptions[message].push_back(pair);
}
void EventManager::UnsubscribeMessage(const std::string &message, void* obj){
	auto search = messageSubscriptions.find(message);
	if(search != messageSubscriptions.end()){
		auto& vector = search->second;
		for (auto it = vector.begin(); it != vector.end(); ) {
			if(it->first == obj){
				it = vector.erase(it);
			} else {
				++it;
			}
		}
	}
}
void EventManager::UnsubscribeMessagesAll(void* obj){
	auto it = messageSubscriptions.begin();
	while(it != messageSubscriptions.end()){
		auto& vector = it->second;
		for (auto it = vector.begin(); it != vector.end(); ) {
			if(it->first == obj){
				it = vector.erase(it);
			} else {
				++it;
			}
		}
		it++;
	}
}



}

