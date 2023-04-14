#include "Client.h"
#include "events/EventManager.h"
#include "events/Event.h"

using namespace std;

namespace Crawler {

Client::Client(int id){
	this->id = id;
	connected = true;
	for(int i = 0; i < static_cast<int>(KeyCode::KEY_CODE_MAX); i++){
		isDownMap[i] = false;
		onDownMap[i] = false;
		onUpMap[i] = false;
	}
}

void Client::AddMessage(const string& str){
	shared_ptr<Event> event = EventManager::ParseMessage(str);
	if(event != nullptr){
		event->fromClient = true;
		event->clientId = this->id;
		newEvents.push_back(event);
		EventManager::AddEvent(event);
	}
}

void Client::Update(){
	for(int i = 0; i < static_cast<int>(KeyCode::KEY_CODE_MAX); i++){
		onDownMap[i] = onUpMap[i] = false;
	}
	for(auto const &value: events){
		switch(value->type){
		case EventType::Input: {
			shared_ptr<InputEvent> inputEvent = static_pointer_cast<InputEvent>(value);
			switch(inputEvent->state){
				case KeyState::Pressed: {
					isDownMap[static_cast<int>(inputEvent->code)] = true;
					onDownMap[static_cast<int>(inputEvent->code)] = true;
					break;
				}
				case KeyState::Released: {
					isDownMap[static_cast<int>(inputEvent->code)] = false;
					onUpMap[static_cast<int>(inputEvent->code)] = true;
					break;
				}
				default: break;
			}
			break;
		}
		case EventType::Joystick: {
			shared_ptr<JoystickEvent> joy = static_pointer_cast<JoystickEvent>(value);
			switch(joy->state){
				case KeyState::JoystickMove: {
					break;
				}
				case KeyState::Pressed: {
					isDownMap[static_cast<int>(joy->code)] = true;
					onDownMap[static_cast<int>(joy->code)] = true;
					break;
				}
				case KeyState::Released: {
					isDownMap[static_cast<int>(joy->code)] = false;
					onUpMap[static_cast<int>(joy->code)] = true;
					break;
				}
			}
			int id = joy->code == KeyCode::LeftJoystick ? 0 : 1;
			joystickPositions[id] = joy->position;
			break;
		}
		}
	}
}

void Client::PopNewEvents(){
	events.clear();
	events.insert(events.end(), newEvents.begin(), newEvents.end());
	newEvents.clear();
}

bool Client::IsKeyDown(KeyCode code){
	return isDownMap[static_cast<int>(code)];
}

bool Client::OnKeyDown(KeyCode code){
	return onDownMap[static_cast<int>(code)];
}

bool Client::OnKeyUp(KeyCode code){
	return onUpMap[static_cast<int>(code)];
}

Eigen::Vector2f Client::GetJoystickPosition(KeyCode code){
	int id = (code == KeyCode::LeftJoystick) ? 0 : 1;
	return joystickPositions[id];
}

void Client::SetFlag(uint32_t flag){
	flags |= (1<<flag);
}
void Client::UnsetFlag(uint32_t flag){
	flags &= ~(1<<flag);
}
bool Client::IsFlagSet(uint32_t flag){
	return (flags&(1<<flag)) > 0;
}

}


