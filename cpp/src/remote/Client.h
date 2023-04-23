#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"
#include "GamepadKeys.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace Crawler {

class Packet;

class Client {
public:
	int id;
	uint32_t flags = 0;
	bool connected;

	std::vector<std::shared_ptr<Packet>> packets;
	std::vector<std::shared_ptr<Packet>> newPackets;

	Client(int id);
	void Update();
	bool IsKeyDown(GamepadKey code);
	bool OnKeyDown(GamepadKey code);
	bool OnKeyUp(GamepadKey code);
	Eigen::Vector2f GetJoystickPosition(GamepadKey code);

	void QueuePacket(std::shared_ptr<Packet> packet);
	void ReceivePackets();
	void SendPacket(std::shared_ptr<Packet> packet); 

	void SetFlag(uint32_t flag);
	void UnsetFlag(uint32_t flag);
	bool IsFlagSet(uint32_t flag);

private:
	std::vector<GamepadKey> downKeys;
	std::vector<GamepadKey> upKeys;
	bool isDownMap[NUM_GAMEPAD_KEYS];
	bool onDownMap[NUM_GAMEPAD_KEYS];
	bool onUpMap[NUM_GAMEPAD_KEYS];
	Eigen::Vector2f joystickPositions[NUM_GAMEPAD_JOYSTICKS];
};

}


