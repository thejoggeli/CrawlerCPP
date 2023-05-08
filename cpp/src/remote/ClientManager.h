#pragma once

#include <vector>
#include <memory>
#include "GamepadKeys.h"
#include "PacketType.h"
#include "core/Log.h"

namespace Crawler {

class Client;
class Packet;
class PacketMessage;

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
	static std::shared_ptr<Client> GetOldestClient();
	static std::shared_ptr<Client> GetNewestClient();

	static bool IsKeyDown(GamepadKey code);
	static bool OnKeyDown(GamepadKey code);
	static bool OnKeyUp(GamepadKey code);

	static void SubscribePacket(PacketType type, void* obj, void (*handler)(void*, Packet&));
	static void UnsubscribePacket(PacketType type, void* obj);

	static void SubscribeMessage(const char* type, void* obj, void (*handler)(void*, PacketMessage&));
	static void UnsubscribeMessage(const char* type, void* obj);

	static void SendPacket(std::shared_ptr<Packet> packet, int clientId = -1);
	static void SendMessage(const char* message, int clientId = -1);
	
	static void SendLogInfo(const char* from, const char* str, int clientId = -1);
	static void SendLogError(const char* from, const char* str, int clientId = -1);
	static void SendLogWarning(const char* from, const char* str, int clientId = -1);
	static void SendLogDebug(const char* from, const char* str, int clientId = -1);
	
	static void SendLogInfo(const char* from, LogConcator& log, int clientId = -1);
	static void SendLogError(const char* from, LogConcator& log, int clientId = -1);
	static void SendLogWarning(const char* from, LogConcator& log, int clientId = -1);
	static void SendLogDebug(const char* from, LogConcator& log, int clientId = -1);

	static void ReceivePackets();
};


}

