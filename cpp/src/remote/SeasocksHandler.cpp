#include "SeasocksHandler.h"
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
	logInput = Config::GetBool("log_info_server_input", true);
	logOutput = Config::GetBool("log_info_server_output", true);
}

void SeasocksHandler::onConnect(WebSocket* connection) {
	int clientId = ClientManager::GenerateId();
	// alloc memory for client
	shared_ptr<Client> client = make_shared<Client>(clientId);
	// add client to client manager
	ClientManager::AddClient(client);
	// add connection to vector
	connections.push_back(connection);
	// add connection to map
	clientByConnection[connection] = client;
	connectionByClientId[client->id] = connection;
};

void SeasocksHandler::onData(WebSocket* connection, const uint8_t* data, size_t size) {
	if(logInput){
		LogDebug("SeasocksHandler", iLog << "OnData (size=" << size << ")");
	}
	shared_ptr<Client> client = GetClientByConnection(connection);
	if(!client){
		LogDebug("SeasocksHandler", iLog << "GetClientByConnection() failed in onData()");
		return;
	}
	shared_ptr<Packet> packet = make_shared<Packet>(data, size);
	client->QueuePacket(packet);
};

void SeasocksHandler::onDisconnect(WebSocket* connection) {
	shared_ptr<Client> client = GetClientByConnection(connection);
	if(client == nullptr){
		Log(LOG_ERROR, "SeasocksHandler", "Client not found in client map");
		return;
	}
	// remove client from client manager
	ClientManager::RemoveClient(client->id);
	// remove connection from vector
	auto it = std::find(connections.begin(), connections.end(), connection);
	connections.erase(it);
	// remove connection from map
	clientByConnection.erase(connection);
	connectionByClientId.erase(client->id);
};

shared_ptr<Client> SeasocksHandler::GetClientByConnection(WebSocket* connection){
	auto search = clientByConnection.find(connection);
	if(search == clientByConnection.end()){
		return nullptr;
	}
	return search->second;
}

shared_ptr<Client> SeasocksHandler::GetClientById(int id){
	return ClientManager::GetClient(id);
}

seasocks::WebSocket* SeasocksHandler::GetConnectionByClientId(int id){
	if(connectionByClientId.contains(id)){
		return connectionByClientId[id];
	}
	return nullptr;
}

}