#include "Client.h"
#include "core/Log.h"
#include "Packet.h"
#include "SocketServer.h"

using namespace std;

namespace Crawler {

Client::Client(int id){
	this->id = id;
	connected = true;
	for(int i = 0; i < NUM_GAMEPAD_KEYS; i++){
		isDownMap[i] = false;
		onDownMap[i] = false;
		onUpMap[i] = false;
	}
}

void Client::QueuePacket(std::shared_ptr<Packet> packet){ 
	newPackets.push_back(packet);
}

void Client::ReceivePackets(){
	packets = newPackets;
	newPackets.clear();
}

void Client::SendPacket(std::shared_ptr<Packet> packet){
	SocketServer::SendPacket(packet, id);
}

void Client::Update(){
	for(int i = 0; i < NUM_GAMEPAD_KEYS; i++){
		onDownMap[i] = onUpMap[i] = false;
	}
	for(auto& packet: packets){
		switch(packet->type){
		case PacketType::GamepadKey: {
			GamepadKey key = (GamepadKey) packet->data.Get<uint8_t>("key");
			GamepadKeyState state = (GamepadKeyState) packet->data.Get<uint8_t>("state");
			switch(state){
				case GamepadKeyState::Pressed: {
					isDownMap[(unsigned int)key] = true;
					onDownMap[(unsigned int)key] = true;
					break;
				}
				case GamepadKeyState::Released: {
					isDownMap[(unsigned int)key] = false;
					onUpMap[(unsigned int)key] = true;
					break;
				}
				default: break;
			}
			break;
		}
		case PacketType::GamepadJoystick: {
			GamepadKey key = (GamepadKey) packet->data.Get<uint8_t>("key");
			GamepadKeyState state = (GamepadKeyState) packet->data.Get<uint8_t>("state");
			switch(state){
				case GamepadKeyState::Pressed: {
					isDownMap[(unsigned int)key] = true;
					onDownMap[(unsigned int)key] = true;
					break;
				}
				case GamepadKeyState::Released: {
					isDownMap[(unsigned int)key] = false;
					onUpMap[(unsigned int)key] = true;
					break;
				}
				default: break;
			}
			unsigned int id = (unsigned int)(key) & GAMEPAD_JOYSTICK_MASK;
			float x = packet->data.Get<float>("x");
			float y = packet->data.Get<float>("y");
			joystickPositions[id] = Eigen::Vector2f(x, y);
			break;
		}
		}
	}
}

bool Client::IsKeyDown(GamepadKey code){
	return isDownMap[static_cast<int>(code)];
}

bool Client::OnKeyDown(GamepadKey code){
	return onDownMap[static_cast<int>(code)];
}

bool Client::OnKeyUp(GamepadKey code){
	return onUpMap[static_cast<int>(code)];
}

Eigen::Vector2f Client::GetJoystickPosition(GamepadKey code){
	int id = (code == GamepadKey::LeftJoystick) ? 0 : 1;
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


