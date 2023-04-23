#include "SocketServer.h"
#include "SocketMessage.h"
#include "ClientManager.h"
#include "Client.h"
#include "core/Config.h"
#include "core/Log.h"
#include <algorithm>
#include "Packet.h"
#include <seasocks/Server.h>
#include <seasocks/WebSocket.h>
#include <seasocks/StringUtil.h>
#include <seasocks/PrintfLogger.h>
#include "SeasocksHandler.h"
#include <unordered_map>


using namespace seasocks;
using namespace std;

namespace Crawler {


static std::vector<seasocks::WebSocket*> connections;
static std::unordered_map<seasocks::WebSocket*, std::shared_ptr<Client>> clientByConnection;
static std::unordered_map<int, seasocks::WebSocket*> connectionByClientId;

static std::shared_ptr<seasocks::Server> server;
static std::shared_ptr<seasocks::PrintfLogger> logger;
static std::shared_ptr<SeasocksHandler> handler;

bool SocketServer::logInput;
bool SocketServer::logOutput;
int SocketServer::port;
std::string SocketServer::websocketPath;
std::string SocketServer::localUrl;
std::string SocketServer::globalUrl;

SocketServer::SocketServer(){}

bool SocketServer::Init(){

	logInput = Config::GetBool("log_info_server_input", true);
	logOutput = Config::GetBool("log_info_server_output", true);

	Logger::Level loggerLevel = static_cast<Logger::Level>(Config::GetInt("server_logger_level"));
	port = Config::GetInt("server_port");
	websocketPath = "/ws";
	localUrl = "ws://localhost:" + to_string(port) + websocketPath;
	globalUrl = "undefined";

	// logger
	logger = std::make_shared<PrintfLogger>(loggerLevel);

	// server
	std::string staticPath = CRAWLER_WEB_PATH;
	server = make_shared<seasocks::Server>(logger);
//	server->setClientBufferSize(1024*1024); // 1MB
	server->setStaticPath(staticPath.c_str());
	handler = make_shared<SeasocksHandler>(server);
	server->addWebSocketHandler(websocketPath.c_str(), handler);
	server->startListening(port);

	// output
	Log(LOG_DEBUG, "Server", iLog << "StaticPath: " << CRAWLER_WEB_PATH);
	Log(LOG_DEBUG, "Server", iLog << "LoggerLevel set to " << Logger::levelToString(loggerLevel));
	Log(LOG_INFO, "Server", iLog << "Local access: " << GetLocalUrl());
	Log(LOG_INFO, "Server", iLog << "Global access: " << GetGlobalUrl());

	return true;
}

void SocketServer::OnConnect(WebSocket* connection){
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
}

void SocketServer::OnData(WebSocket* connection, const uint8_t* data, size_t size){
	if(logInput){
		LogDebug("SeasocksHandler", iLog << "OnData (size=" << size << ")");
	}
	shared_ptr<Client> client = GetClientByConnection(connection);
	if(!client){
		LogDebug("SeasocksHandler", iLog << "GetClientByConnection() failed in onData()");
		return;
	}
	shared_ptr<Packet> packet = Packet::Unpack(data, size);
	if(packet){
		client->QueuePacket(packet);
	}
}

void SocketServer::OnDisconnect(WebSocket* connection){
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
}

void SocketServer::Poll(){
	server->poll(0);
}

int SocketServer::GetNumConnections(){
	return static_cast<int>(connections.size());
}

int SocketServer::GetPort(){
	return port;
}

string SocketServer::GetLocalUrl(){
	return localUrl;
}

string SocketServer::GetGlobalUrl(){
	return globalUrl;
}
void SocketServer::Shutdown(){
	for(auto const& it: connections){
		it->close();
	}
	server->terminate();
	server = nullptr;
}

void SocketServer::SendPacket(std::shared_ptr<Packet> packet, int clientId){
	LogDebug("SocketServer", iLog << "Send (id=" << clientId << ") " << *PacketTypeToString(packet->type));
	if(clientId == -1){
		for(auto const& connection: connections){
			connection->send(packet->data.GetBytes(), packet->data.GetSize());
		}
	} else {
		auto const& connection = GetConnectionByClientId(clientId);
		if(connection){
			connection->send(packet->data.GetBytes(), packet->data.GetSize());
		}
	}
}

// void SocketServer::Send(std::string& string){
// 	for(auto const& it: handler->connections){
// 		it->send(string);
// 	}
// }
// void SocketServer::SendMessage(SocketMessage& message, int clientId){
// 	string& str = message.GetJsonString();
// 	Log(LOG_DEBUG, "Server", iLog << "Send (id=" << clientId << ") " << str);
// 	if(clientId == 0){
// 		for(auto const& it: handler->connections){
// 			it->send(str);
// 		}
// 	} else {
// 		for(auto const& it: handler->connections){
// 			shared_ptr<Client> c = handler->GetClientByConnection(it);
// 			if(c && c->id == clientId){
// 				it->send(str);
// 			}
// 		}
// 	}
// }

shared_ptr<Client> SocketServer::GetClientByConnection(WebSocket* connection){
	auto search = clientByConnection.find(connection);
	if(search == clientByConnection.end()){
		return nullptr;
	}
	return search->second;
}

shared_ptr<Client> SocketServer::GetClientById(int id){
	return ClientManager::GetClient(id);
}

seasocks::WebSocket* SocketServer::GetConnectionByClientId(int id){
	if(connectionByClientId.contains(id)){
		return connectionByClientId[id];
	}
	return nullptr;
}

}

