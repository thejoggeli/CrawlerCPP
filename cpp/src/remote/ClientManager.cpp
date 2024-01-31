#include "ClientManager.h"
#include "Client.h"
#include "core/Log.h"
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "Packet.h"
#include "SocketServer.h"
#include <mutex>

using namespace std;

namespace Crawler {

struct SendMessageQueueEntry {
	shared_ptr<PacketMessage> packet;
	int clientId;
};

static vector<SendMessageQueueEntry> sendMessageQueue;

int ClientManager::initCounter = 0;

static std::mutex mutex;
static int currentClientId = 0;
static vector<shared_ptr<Client>> clientsVector;
static unordered_map<int, shared_ptr<Client>> clientsMap;
static std::unordered_map<PacketType, std::vector<std::pair<void*, void (*)(void*, Packet&)>>> packetSubscriptions;
static std::unordered_map<std::string, std::vector<std::pair<void*, void (*)(void*, PacketMessage&)>>> packetMessageSubscriptions;

bool ClientManager::Init(){
	if(++initCounter > 1) return false;
	Log(LOG_INFO, "ClientManager", "Initializing");
	return true;
}

void ClientManager::Update(){
	for(auto const &client: GetAllCients()){
		client->Update();
	}
}

void ClientManager::AddClient(const shared_ptr<Client>& client){
	clientsVector.push_back(client);
	clientsMap.insert({client->id, client});
/*	if(iLog.infos.clients){
		Log(LOG_INFO, "ClientManager", iLog << "Client (" << client->id << ") added");
	} */
}

void ClientManager::RemoveClient(int id){
	shared_ptr<Client> client = GetClient(id);
	if(client == nullptr){
		Log(LOG_ERROR, "ClientManager", iLog << "Can't remove client (id=" << id << "): not found in map");
		return;
	}	
	vector<shared_ptr<Client>>::iterator it = find_if(clientsVector.begin(), clientsVector.end(), [&id](shared_ptr<Client>& c){
		return c->id == id;
	});
	if(it == clientsVector.end()){
		Log(LOG_ERROR, "ClientManager", "Client found in map but not found in vector");
		return;
	}
	clientsVector.erase(it);
	clientsMap.erase(id);
/*	if(iLog.infos.clients){
		Log(LOG_INFO, "ClientManager", iLog << "Client removed: " << client->id);
	} */
}

shared_ptr<Client> ClientManager::GetClient(int id){
	auto search = clientsMap.find(id);
	if(search != clientsMap.end()){
		return search->second;
	}
	return nullptr;
};

std::shared_ptr<Client> ClientManager::GetOldestClient(){
	if(clientsVector.size() > 0){
		return clientsVector[0];
	}
	return nullptr;
}

std::shared_ptr<Client> ClientManager::GetNewestClient(){
	if(clientsVector.size() > 0){
		return clientsVector[clientsVector.size()-1];
	}
	return nullptr;
}

const vector<shared_ptr<Client>>& ClientManager::GetAllCients() {
	return clientsVector;
}

bool ClientManager::IsKeyDown(GamepadKey code){
	for(vector<shared_ptr<Client>>::iterator it = clientsVector.begin(); it != clientsVector.end(); ++it){
		if((*it)->IsKeyDown(code)) return true;
	}
	return false;
};
bool ClientManager::OnKeyDown(GamepadKey code){
	for(vector<shared_ptr<Client>>::iterator it = clientsVector.begin(); it != clientsVector.end(); ++it){
		if((*it)->OnKeyDown(code)) return true;
	}
	return false;
};
bool ClientManager::OnKeyUp(GamepadKey code){
	for(vector<shared_ptr<Client>>::iterator it = clientsVector.begin(); it != clientsVector.end(); ++it){
		if((*it)->OnKeyUp(code)) return true;
	}
	return false;
};

int ClientManager::GenerateId(){
	return currentClientId++;
}

void ClientManager::SubscribePacket(PacketType type, void* obj, void (*handler)(void*, Packet&)){
	std::pair<void*, void (*)(void*, Packet&)> pair;
	pair = std::make_pair(obj, handler);
	packetSubscriptions[type].push_back(pair);
}

void ClientManager::UnsubscribePacket(PacketType type, void* obj){
	auto search = packetSubscriptions.find(type);
	if(search != packetSubscriptions.end()){
		auto& vector = search->second;
		for (auto it = vector.begin(); it != vector.end(); ) {
			if(it->first == obj){
				it = vector.erase(it);
			} else {
				++it;
			}
		}
	}
}

void ClientManager::SubscribeMessage(const char* type, void* obj, void (*handler)(void*, PacketMessage&)){
	std::pair<void*, void (*)(void*, PacketMessage&)> pair;
	pair = std::make_pair(obj, handler);
	packetMessageSubscriptions[type].push_back(pair);
}

void ClientManager::UnsubscribeMessage(const char* type, void* obj){
	auto search = packetMessageSubscriptions.find(type);
	if(search != packetMessageSubscriptions.end()){
		auto& vector = search->second;
		for (auto it = vector.begin(); it != vector.end(); ) {
			if(it->first == obj){
				it = vector.erase(it);
			} else {
				++it;
			}
		}
	}
}

void ClientManager::SendPacket(std::shared_ptr<Packet> packet, int clientId){
	std::lock_guard<std::mutex> lock(mutex);
	SocketServer::SendPacket(packet, clientId);
}

void ClientManager::SendMessage(const char* message, int clientId){
	std::lock_guard<std::mutex> lock(mutex);
    std::shared_ptr<PacketMessage> packet = std::make_shared<PacketMessage>(message);
	SocketServer::SendPacket(packet, clientId);
}

void ClientManager::SendLogMessages(){
	std::lock_guard<std::mutex> lock(mutex);
	for(SendMessageQueueEntry& entry : sendMessageQueue){
		SocketServer::SendPacket(entry.packet, entry.clientId);
	}
	sendMessageQueue.clear();
}

static void SendLogInner(const char* from, const char* str, int clientId, const char* type){
    std::shared_ptr<PacketMessage> packet = std::make_shared<PacketMessage>("log");
	packet->AddString("type", type);
	packet->AddString("from", from);
	packet->AddString("msg", str);
	SendMessageQueueEntry entry;
	entry.packet = packet;
	entry.clientId = clientId;
	sendMessageQueue.push_back(entry);
}

void ClientManager::SendLogInfo(const char* from, const char* str, int clientId){
	std::lock_guard<std::mutex> lock(mutex);
	SendLogInner(from, str, clientId, "info");
	LogInfo(from, str);
}

void ClientManager::SendLogWarning(const char* from, const char* str, int clientId){
	std::lock_guard<std::mutex> lock(mutex);
	SendLogInner(from, str, clientId, "warning");
	LogWarning(from, str);
}

void ClientManager::SendLogError(const char* from, const char* str, int clientId){
	std::lock_guard<std::mutex> lock(mutex);
	SendLogInner(from, str, clientId, "error");
	LogError(from, str);
}

void ClientManager::SendLogDebug(const char* from, const char* str, int clientId){
	std::lock_guard<std::mutex> lock(mutex);
	SendLogInner(from, str, clientId, "debug");
	LogDebug(from, str);
}

void ClientManager::SendLogInfo(const char* from, LogConcator& log, int clientId){
	std::lock_guard<std::mutex> lock(mutex);
	std::string str = log.stringStream.str();
	SendLogInner(from, str.c_str(), clientId, "info");
	LogInfo(from, str);
}

void ClientManager::SendLogWarning(const char* from, LogConcator& log, int clientId){
	std::lock_guard<std::mutex> lock(mutex);
	std::string str = log.stringStream.str();
	SendLogInner(from, str.c_str(), clientId, "warning");
	LogWarning(from, str);
}

void ClientManager::SendLogError(const char* from, LogConcator& log, int clientId){
	std::lock_guard<std::mutex> lock(mutex);
	std::string str = log.stringStream.str();
	SendLogInner(from, str.c_str(), clientId, "error");
	LogError(from, str);
}

void ClientManager::SendLogDebug(const char* from, LogConcator& log, int clientId){
	std::lock_guard<std::mutex> lock(mutex);
	std::string str = log.stringStream.str();
	SendLogInner(from, str.c_str(), clientId, "debug");
	LogDebug(from, str);
}

void ClientManager::ReceivePackets(){
	// LogDebug("ClientManager", iLog << "receiving packets");
	for(auto const &client: GetAllCients()){
		client->ReceivePackets();
	}
	for(auto const &client: GetAllCients()){
		for(std::shared_ptr<Packet> packet: client->packets){
			// LogDebug("ClientManager", iLog << "receiving packet: " << *PacketTypeToString(packet->type));
			// fire Packet subscriptions
			auto search = packetSubscriptions.find(packet->type);
			if (search != packetSubscriptions.end()){
				auto& vector = search->second;
				for(auto& handler: vector){
					handler.second(handler.first, *packet);
				}
			}
			// check if packet is of type Message
			if(packet->type == PacketType::Message){
				// fire PacketMessage subscritions
				PacketMessage* pm = (PacketMessage*)(packet.get());
				auto search = packetMessageSubscriptions.find(pm->message);
				if (search != packetMessageSubscriptions.end()){
					auto& vector = search->second;
					for(auto& handler: vector){
						handler.second(handler.first, *pm);
					}
				}
			}
		}
	}
}

}
