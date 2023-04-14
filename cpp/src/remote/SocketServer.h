#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include "SocketMessage.h"

namespace seasocks {
	class Server;
	class PrintfLogger;
}


namespace Crawler {

class Client;
class SeasocksHandler;

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
	static void Send(std::string& string);
	static void SendMessage(SocketMessage& message, int clientId);
	static void Shutdown();
};

}
