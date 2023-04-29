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
	// LogDebug("Client", iLog << "Queue packet type=" << *PacketTypeToString(packet->type));
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
	
	// clear OnKeyDown() state from last frame
	for(GamepadKey key : downKeys){
		onDownMap[(unsigned int)key] = false;
	}
	downKeys.clear();

	// clear OnKeyUp() state from last frame
	for(GamepadKey key : upKeys){
		onUpMap[(unsigned int)key] = false;
	}
	upKeys.clear();

	// process packets
	for(auto& packet: packets){
		switch(packet->type){
		case PacketType::GamepadKey: {
			auto p = std::static_pointer_cast<PacketGamepadKey>(packet);
			switch(p->state){
				case GamepadKeyState::Pressed: {
					isDownMap[(unsigned int)p->key] = true;
					onDownMap[(unsigned int)p->key] = true;
					downKeys.push_back(p->key);
					break;
				}
				case GamepadKeyState::Released: {
					isDownMap[(unsigned int)p->key] = false;
					onUpMap[(unsigned int)p->key] = true;
					upKeys.push_back(p->key);
					break;
				}
				default: break;
			}
			break;
		}
		case PacketType::GamepadJoystick: {
			auto p = std::static_pointer_cast<PacketGamepadJoystick>(packet);
			unsigned int id = (unsigned int)(p->key) & GAMEPAD_JOYSTICK_MASK;
			// LogDebug("Client", iLog << "x: " << x << ", y: " << y);
			joystickPositions[id] = Eigen::Vector2f(p->x, p->y);
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
	int id = (uint8_t)(code) & GAMEPAD_JOYSTICK_MASK;
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


