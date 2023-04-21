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

void SocketServer::SendPacket(std::shared_ptr<Packet> packet, int clientId){
	LogDebug("SocketServer", iLog << "Send (id=" << clientId << ") " << *PacketTypeToString(packet->type));
	if(clientId == -1){
		for(auto const& connection: handler->connections){
			connection->send(packet->data.GetBytes(), packet->data.GetSize());
		}
	} else {
		auto const& connection = handler->GetConnectionByClientId(clientId);
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

}

