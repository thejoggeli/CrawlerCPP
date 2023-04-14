#include "ClientManager.h"
#include "Client.h"
#include "core/Log.h"
#include <algorithm>
#include <unordered_map>
#include <vector>

using namespace std;

namespace Crawler {

int ClientManager::initCounter = 0;

static int currentClientId = 1;
static vector<shared_ptr<Client>> clientsVector;
static unordered_map<int, shared_ptr<Client>> clientsMap;

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

bool ClientManager::IsKeyDown(KeyCode code){
	for(vector<shared_ptr<Client>>::iterator it = clientsVector.begin(); it != clientsVector.end(); ++it){
		if((*it)->IsKeyDown(code)) return true;
	}
	return false;
};
bool ClientManager::OnKeyDown(KeyCode code){
	for(vector<shared_ptr<Client>>::iterator it = clientsVector.begin(); it != clientsVector.end(); ++it){
		if((*it)->OnKeyDown(code)) return true;
	}
	return false;
};
bool ClientManager::OnKeyUp(KeyCode code){
	for(vector<shared_ptr<Client>>::iterator it = clientsVector.begin(); it != clientsVector.end(); ++it){
		if((*it)->OnKeyUp(code)) return true;
	}
	return false;
};

int ClientManager::GenerateId(){
	return currentClientId++;
}

void ClientManager::PopNewEvents(){
	for(auto const &client: GetAllCients()){
		client->PopNewEvents();
	}
}

}
