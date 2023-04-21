#pragma once

#include <seasocks/Server.h>
#include <seasocks/WebSocket.h>
#include <unordered_map>

namespace Crawler {

class Client;

class SeasocksHandler : public seasocks::WebSocket::Handler {
private:

	bool logInput = false;
	bool logOutput = false;
	
	std::unordered_map<seasocks::WebSocket*, std::shared_ptr<Client>> clientByConnection;
	std::unordered_map<int, seasocks::WebSocket*> connectionByClientId;

public:

	std::shared_ptr<seasocks::Server> server;
	std::vector<seasocks::WebSocket*> connections;

	explicit SeasocksHandler(std::shared_ptr<seasocks::Server> server);
	virtual void onConnect(seasocks::WebSocket* connection) override;
	virtual void onData(seasocks::WebSocket* connection, const uint8_t* data, size_t size) override;
	virtual void onDisconnect(seasocks::WebSocket* connection) override;
	std::shared_ptr<Client> GetClientByConnection(seasocks::WebSocket* connection);
	std::shared_ptr<Client> GetClientById(int id);
	seasocks::WebSocket* GetConnectionByClientId(int id);
};

}