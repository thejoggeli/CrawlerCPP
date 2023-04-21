#include "SeasocksHandler.h"
#include "events/EventManager.h"
#include "events/Event.h"
#include "core/Config.h"
#include "core/Log.h"
#include "ClientManager.h"
#include "Client.h"

using namespace seasocks;
using namespace std;

namespace Crawler {

SeasocksHandler::SeasocksHandler(shared_ptr<seasocks::Server> server){
	this->server = server;
	logInput = Config::GetBool("log_info_server_input", true);
	logOutput = Config::GetBool("log_info_server_output", true);
}

void SeasocksHandler::onConnect(WebSocket* connection) {
	int clientId = ClientManager::GenerateId();
	// alloc memory for client
	shared_ptr<Client> client = make_shared<Client>(clientId);
	// add client to client manager
	ClientManager::AddClient(client);
	// broadcast client connected event
	shared_ptr<MessageEvent> event = make_shared<MessageEvent>("client_connected");
	event->clientId = client->id;
	event->fromClient = false;
	EventManager::AddEvent(event);
	// add connection to vector
	connections.push_back(connection);
	// add connection to map
	clientMap[connection] = client;
};

void SeasocksHandler::onData(WebSocket* connection, const char* data) {
	if(logInput){
		Log(LOG_INFO, "Server", iLog << "OnData: " << data);
	}
	shared_ptr<Client> client = GetClientByConnection(connection);
	client->AddMessage(data);
};

void SeasocksHandler::onDisconnect(WebSocket* connection) {
	shared_ptr<Client> client = GetClientByConnection(connection);
	if(client == nullptr){
		Log(LOG_ERROR, "Server", "Client not found in client map");
		return;
	}
	// remove client from client manager
	ClientManager::RemoveClient(client->id);
	// broadcast client connected event
	shared_ptr<MessageEvent> event = make_shared<MessageEvent>("client_disconnected");
	event->clientId = client->id;
	event->fromClient = false;
	EventManager::AddEvent(event);
	// remove connection from vector
	auto it = std::find(connections.begin(), connections.end(), connection);
	connections.erase(it);
	// remove connection from map
	clientMap.erase(connection);
};

shared_ptr<Client> SeasocksHandler::GetClientByConnection(WebSocket* connection){
	auto search = clientMap.find(connection);
	if(search == clientMap.end()){
		return nullptr;
	}
	return search->second;
}

}