#pragma once

#include <vector>
#include <memory>
#include "SocketMessage.h"
#include "PacketType.h"

namespace seasocks {
	class Server;
	class PrintfLogger;
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

	static std::shared_ptr<seasocks::Server> server;
	static std::shared_ptr<seasocks::PrintfLogger> logger;
	static std::shared_ptr<SeasocksHandler> handler;

    SocketServer();
public:
    
	static bool Init();
	static void Poll();
    
	static int GetNumConnections();
    static int GetPort();
	static std::string GetLocalUrl();
	static std::string GetGlobalUrl();

	static void Shutdown();

	static void SendPacket(std::shared_ptr<Packet> packet, int clientId);

};

}
