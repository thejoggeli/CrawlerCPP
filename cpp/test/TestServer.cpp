
#include "core/Log.h"
#include "core/Time.h"
#include "core/Config.h"
#include "remote/SocketServer.h"
#include "remote/ClientManager.h"
#include "remote/Client.h"
#include "util/Timer.h"
#include <csignal>
#include "util/Endian.h"
#include "remote/Packet.h"

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

int main(){

    LogInfo("Endian", iLog << "IsBigEndian: " << IsBigEndian());

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

    Time::Start();

    Timer timer;
    timer.Start(1.0f);

    while(!exitRequested){

        Time::Update();

        // poll socket server
        SocketServer::Poll();
        
        // receive new packets
        ClientManager::ReceivePackets();
        
        // update clients
        ClientManager::Update();

        // send packet
        if(false && timer.IsFinished()){
            timer.Restart(true);
            std::shared_ptr<Packet> packet = std::make_shared<Packet>(PacketType::SC_ReadRobotPosLegAngles);
            packet->data.Add<uint8_t>(4); // num legs
            for(int i = 0; i < 4; i++){
                packet->data.Add<uint8_t>(i); // leg id
                for(int j = 0; j < 4; j++){
                    packet->data.Add<float>(j*0.5f); // joint angles
                }
            }
            packet->data.PrintPretty(true, 16);
            ClientManager::SendPacket(packet);
        }

        // test client input
        if(false){
            shared_ptr<Client> client = ClientManager::GetOldestClient();
            if(client){
                LogInfo("TestServer", iLog 
                    << "A: " << client->IsKeyDown(GamepadKey::A) << ", "
                    << "J: " << client->IsKeyDown(GamepadKey::LeftJoystick) << ", "
                    << "xy: " << client->GetJoystickPosition(GamepadKey::LeftJoystick).transpose()
                );
            }
        }

        // test client input 2
        if(true){
            shared_ptr<Client> client = ClientManager::GetOldestClient();
            if(client){
                if(client->OnKeyDown(GamepadKey::A)){
                    LogDebug("TestServer", "A OnKeyDown");
                } else if(client->IsKeyDown(GamepadKey::A)){
                    LogDebug("TestServer", "A IsKeyDown");
                } else if(client->OnKeyUp(GamepadKey::A)){
                    LogDebug("TestServer", "A OnKeyUp");
                }
            }
        }

        Time::SleepMicros(1000);

    }

    return EXIT_SUCCESS;

}