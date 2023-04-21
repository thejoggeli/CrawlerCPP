
#include "core/Log.h"
#include "core/Time.h"
#include "core/Config.h"
#include "remote/SocketServer.h"
#include "remote/ClientManager.h"
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

bool is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1;
}

int main(){

    LogInfo("Endian", iLog << "is_big_endian: " << is_big_endian);

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
        
        // receive new packets
        ClientManager::ReceivePackets();
        
        // update clients
        ClientManager::Update();

    }

    return EXIT_SUCCESS;

}