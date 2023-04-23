#include "SeasocksHandler.h"
#include "SocketServer.h"
#include "core/Config.h"
#include "core/Log.h"
#include "ClientManager.h"
#include "Client.h"
#include "Packet.h"

using namespace seasocks;
using namespace std;

namespace Crawler {

SeasocksHandler::SeasocksHandler(shared_ptr<seasocks::Server> server){
	this->server = server;
}

void SeasocksHandler::onConnect(WebSocket* connection) {
	SocketServer::OnConnect(connection);
};

void SeasocksHandler::onData(WebSocket* connection, const uint8_t* data, size_t size) {
	SocketServer::OnData(connection, data, size);
};

void SeasocksHandler::onDisconnect(WebSocket* connection) {
	SocketServer::OnDisconnect(connection);
};

}