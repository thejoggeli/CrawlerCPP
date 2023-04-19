
#include "core/Log.h"
#include "core/Time.h"
#include "core/Config.h"
#include "remote/SocketServer.h"
#include "remote/ClientManager.h"
#include "events/EventManager.h"
#include <csignal>

using namespace std;
using namespace Crawler;

bool exitRequested = false;
int sigIntCounter = 0;

void mySigIntHandler(int s){
    exitRequested = true;
    sigIntCounter++;
    if(sigIntCounter >= 3){
        exit(EXIT_FAILURE);
    }
}

bool init(){

    // init config
	if(!Config::Init()){
		LogError("Main", "Config initialization failed");
		return false;
	}

    // init log levels
    if(!LogLevels::Init()){
		LogError("Main", "LogLevels initialization failed");
		return false;
    }

    // init event manager/ event manager
	if(!EventManager::Init()){
		LogError("Main", "EventManager initialization failed");
		return false;
	}

	// init client manager
	if(!ClientManager::Init()){
		LogError("Main", "ClientManager initialization failed");
		return false;
	}

	// init socket server
	if(!SocketServer::Init()){
		Log(LOG_ERROR, "Main", "ServerManager initialization failed");
		return false;
	}

    return true;

}

int main(){

    // sigint handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = mySigIntHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // init components
    if(!init()){    
        return EXIT_FAILURE;
    }

    while(!exitRequested){

        // poll socket server
        SocketServer::Poll();

        // clear events from last tick
        ClientManager::PopNewEvents();
        EventManager::PopNewEvents();
        
        // update clients
        ClientManager::Update();

        // fire events
        EventManager::Update();
    }

    return EXIT_SUCCESS;

}