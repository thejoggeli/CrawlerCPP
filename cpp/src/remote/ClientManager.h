#pragma once

#include <vector>
#include <memory>
#include "GamepadKeys.h"
#include "PacketType.h"

namespace Crawler {

class Client;
class Packet;

class ClientManager {
private:
	static int initCounter;
	
	ClientManager();

public:

	static int GenerateId();

	static bool Init();
	static void Update();

	static void AddClient(const std::shared_ptr<Client>& client);
	static void RemoveClient(int id);
	static std::shared_ptr<Client> GetClient(int id);
	static const std::vector<std::shared_ptr<Client>>& GetAllCients();

	static bool IsKeyDown(GamepadKey code);
	static bool OnKeyDown(GamepadKey code);
	static bool OnKeyUp(GamepadKey code);

	static void SubscribePacket(PacketType type, void* obj, void (*handler)(void*, const Packet&));
	static void UnsubscribePacket(PacketType type, void* obj);
	static void SendPacket(std::shared_ptr<Packet> packet, int clientId);
	static void ReceivePackets();

};


}

