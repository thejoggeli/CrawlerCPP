#pragma once

#include <vector>
#include <memory>
#include "SocketMessage.h"
#include "PacketType.h"

namespace seasocks {
	class Server;
	class PrintfLogger;
	class WebSocket;
}

namespace Crawler {

class Client;
class SeasocksHandler;
class Packet;

class SocketServer {
private:

    static int port;
	static std::string websocketPath;
	static std::string localUrl;
	static std::string globalUrl;

    SocketServer();

public:

	static bool logInput;
	static bool logOutput;
    
	static bool Init();
	static void Poll();

	static void OnConnect(seasocks::WebSocket* connection);
	static void OnData(seasocks::WebSocket* connection, const uint8_t* data, size_t size);
	static void OnDisconnect(seasocks::WebSocket* connection);
    
	static int GetNumConnections();
    static int GetPort();
	static std::string GetLocalUrl();
	static std::string GetGlobalUrl();

	static void Shutdown();

	static void SendPacket(std::shared_ptr<Packet> packet, int clientId);

	static std::shared_ptr<Client> GetClientByConnection(seasocks::WebSocket* connection);
	static std::shared_ptr<Client> GetClientById(int id);
	static seasocks::WebSocket* GetConnectionByClientId(int id);

};

}
