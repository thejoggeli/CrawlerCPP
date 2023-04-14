#pragma once

#include <seasocks/Server.h>
#include <seasocks/WebSocket.h>

namespace Crawler {

class Client;

class SeasocksHandler : public seasocks::WebSocket::Handler {
private:
	bool logInput = false;
	bool logOutput = false;
public:
	std::shared_ptr<seasocks::Server> server;
	std::vector<seasocks::WebSocket*> connections;
	std::unordered_map<seasocks::WebSocket*, std::shared_ptr<Client>> clientMap;
	explicit SeasocksHandler(std::shared_ptr<seasocks::Server> server);
	virtual void onConnect(seasocks::WebSocket* connection) override;
	virtual void onData(seasocks::WebSocket* connection, const char* data) override;
	virtual void onDisconnect(seasocks::WebSocket* connection) override;
	std::shared_ptr<Client> GetClientByConnection(seasocks::WebSocket* connection);
};

}