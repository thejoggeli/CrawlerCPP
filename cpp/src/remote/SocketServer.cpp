#include "SocketServer.h"
#include "SocketMessage.h"
#include "ClientManager.h"
#include "Client.h"
#include "events/EventManager.h"
#include "events/Event.h"
#include "core/Config.h"
#include "core/Log.h"
#include <algorithm>

#include <seasocks/Server.h>
#include <seasocks/WebSocket.h>
#include <seasocks/StringUtil.h>
#include <seasocks/PrintfLogger.h>
#include "SeasocksHandler.h"

using namespace seasocks;
using namespace std;

namespace Crawler {

int SocketServer::port;
std::string SocketServer::websocketPath;
std::string SocketServer::localUrl;
std::string SocketServer::globalUrl;
std::shared_ptr<seasocks::Server> SocketServer::server;
std::shared_ptr<seasocks::PrintfLogger> SocketServer::logger;
std::shared_ptr<SeasocksHandler> SocketServer::handler;

SocketServer::SocketServer(){}

bool SocketServer::Init(){
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

void SocketServer::Send(std::string& string){
	for(auto const& it: handler->connections){
		it->send(string);
	}
}
void SocketServer::SendMessage(SocketMessage& message, int clientId){
	string& str = message.GetJsonString();
	Log(LOG_DEBUG, "Server", iLog << "Send (id=" << clientId << ") " << str);
	if(clientId == 0){
		for(auto const& it: handler->connections){
			it->send(str);
		}
	} else {
		for(auto const& it: handler->connections){
			shared_ptr<Client> c = handler->GetClientByConnection(it);
			if(c && c->id == clientId){
				it->send(str);
			}
		}
	}
}

void SocketServer::Poll(){
	server->poll(0);
}

int SocketServer::GetNumConnections(){
	return static_cast<int>(handler->connections.size());
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
	for(auto const& it: handler->connections){
		it->close();
	}
	server->terminate();
	server = nullptr;
}

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

