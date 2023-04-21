#include "ClientManager.h"
#include "Client.h"
#include "core/Log.h"
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "Packet.h"
#include "SocketServer.h"

using namespace std;

namespace Crawler {

int ClientManager::initCounter = 0;

static int currentClientId = 0;
static vector<shared_ptr<Client>> clientsVector;
static unordered_map<int, shared_ptr<Client>> clientsMap;
static std::unordered_map<PacketType, std::vector<std::pair<void*, void (*)(void*, const Packet&)>>> packetSubscriptions;

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

void ClientManager::SubscribePacket(PacketType type, void* obj, void (*handler)(void*, const Packet&)){
	std::pair<void*, void (*)(void*, const Packet&)> pair;
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

void ClientManager::SendPacket(std::shared_ptr<Packet> packet, int clientId){
	SocketServer::SendPacket(packet, clientId);
}

void ClientManager::ReceivePackets(){
	for(auto const &client: GetAllCients()){
		client->ReceivePackets();
	}
	for(auto const &client: GetAllCients()){
		for(std::shared_ptr<Packet> packet: client->packets){
			auto search = packetSubscriptions.find(packet->type);
			if (search != packetSubscriptions.end()){
				auto& vector = search->second;
				for(auto& handler: vector){
					handler.second(handler.first, *packet);
				}
			}
		}
	}
}

}
