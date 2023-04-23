#pragma once

#include <seasocks/Server.h>
#include <seasocks/WebSocket.h>
#include <unordered_map>

namespace Crawler {

class Client;

class SeasocksHandler : public seasocks::WebSocket::Handler {
private:

	std::shared_ptr<seasocks::Server> server;

public:

	explicit SeasocksHandler(std::shared_ptr<seasocks::Server> server);
	virtual void onConnect(seasocks::WebSocket* connection) override;
	virtual void onData(seasocks::WebSocket* connection, const uint8_t* data, size_t size) override;
	virtual void onDisconnect(seasocks::WebSocket* connection) override;
};

}