#include "Main.h"

#include <JetsonGPIO.h>
#include "util/HardwareButton.h"
#include "util/MonoLED.h"
#include "util/Timer.h"
#include "robot/Robot.h"
#include "robot/Leg.h"
#include "robot/Joint.h"
#include "core/Log.h"
#include "core/Time.h"
#include "core/Config.h"
#include "remote/SocketServer.h"
#include "remote/ClientManager.h"
#include "remote/Client.h"
#include "brain/SurferBrain.h"
#include "brain/GaitBrain.h"
#include "brain/EmptyBrain.h"

using namespace std;
using namespace Crawler;

namespace Crawler {

Main::Main(){
    mainButton = new HardwareButton(24, 200*1000);
    mainButtonLED = new MonoLED(22); 
}

Main::~Main(){
    delete mainButton;
    delete mainButtonLED;
}

bool Main::Init(){

    if(initialized){
        return false;
    }

    LogInfo("Main", "Init()");

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

    // set gpio mode
    LogInfo("Main", "GPIO setup");
    GPIO::setmode(GPIO::BCM);

    // init main button
    mainButton->Init();
    mainButtonLED->Init(true);

    // create robot
    LogInfo("Main", "create robot");
    robot = new Robot();

    // open servo serial stream
    if(robot->OpenSerialStream("/dev/ttyTHS0")){
        LogInfo("Main", "serial stream open");
    } else {
        LogInfo("Main", "serial stream open failed");
        return false;
    }

    // init success
    return true;
}

bool Main::InitServos(){

    LogInfo("Main", "InitServos()");

    // reboot servos
    robot->RebootServos(3.5f);

    // ping servos
    if(!robot->PingServos()){
        // mark successfully pinged servos
        robot->SetServosLedPolicyUser();
        for(Joint* joint: robot->jointsList){
            if(joint->lastPingServoResult){
               joint->SetServoLedColor(0, 1, 0, 0); 
            }
        }
        return false;
    }

    // print servo status
    robot->UpdateAndPrintServosStatus();

    return true;
}

bool Main::Cleanup(){

    LogInfo("Main", "Cleanup()");

    // close servo serial stream
    LogInfo("Main", "serial stream close");
    robot->CloseSerialStream();

    // main button cleanup
    mainButton->Cleanup();
    mainButtonLED->SetState(false);
    mainButtonLED->Cleanup();

    return true;

}

bool Main::Run(){

    LogInfo("Main", "Run()");

    // frame & sleep stuff
    const uint64_t frameDurationTargetMicros = 1000;
    unsigned int upsCounter = 0;
    unsigned int fixedUpsCounter = 0;
    float ups = 0.0f;
    float fixedUps = 0.0f;
    uint64_t longestDeltaTimeMicros = 0;
    uint64_t totalSleepTimeMicros = 0;

    // status timer 
    Timer statusTimer;
    const float statusTimerInterval = 10.0f;

    // set fixed update rate
    Time::SetFixedDeltaTimeMicros(1000*25);
    uint64_t lastUpdateTimeMicros = 0;

    // start up robot
    robot->Startup();
    robot->UpdateAndPrintServosStatus();
    // robot->SetBrain(new SurferBrain());
    robot->SetBrain(new GaitBrain());
    // robot->SetBrain(new EmptyBrain());

    // the beginning of time
    Time::Start();

    // status timer
    statusTimer.Start(statusTimerInterval);

    // main loop
    LogInfo("Main", "main loop start");
    while(!exitRequested){
        
        // update time
        Time::Update();

        // store longest delta time
        if(Time::deltaTimeMicros > longestDeltaTimeMicros){
            longestDeltaTimeMicros = Time::deltaTimeMicros;
        }

        // poll socket server
        SocketServer::Poll();

        // receive new packets
        ClientManager::ReceivePackets();
        
        // update clients
        ClientManager::Update();

        // update buttons
        mainButton->Update();
        if(mainButton->onPress){
            RequestExit();
        }

        // print debug info
        auto& clients = ClientManager::GetAllCients();

        if(clients.size() > 0){
            Client* client = clients[0].get();
            if(client->OnKeyDown(GamepadKey::Select)){
                robot->UpdateAndPrintServosStatus();
            }
        }

        // update robot
        robot->Update();

        // fixed update
        if(Time::currentTimeMicros - lastUpdateTimeMicros >= Time::fixedDeltaTimeMicros){
            robot->FixedUpdate();
            fixedUpsCounter += 1;

            // add fixed delta time (prevents UPS drift)
            lastUpdateTimeMicros += Time::fixedDeltaTimeMicros;

            // prevent frame debt buildup if actual UPS drops below target UPS
            if(Time::currentTimeMicros - lastUpdateTimeMicros >= Time::fixedDeltaTimeMicros){
                LogDebug("Main", "FixedUpdate frame drift prevention");
                lastUpdateTimeMicros = Time::currentTimeMicros;
            }

        }

        // update status
        upsCounter++;
        if(statusTimer.IsFinished()){
            statusTimer.Restart(true);
            ups = (float) upsCounter / statusTimerInterval;
            fixedUps = (float) fixedUpsCounter / statusTimerInterval;
            float capacity = (float)longestDeltaTimeMicros/(float)Time::fixedDeltaTimeMicros;
            float totalSleepTime = (float) totalSleepTimeMicros * 1.0e-6;
            LogInfo("Main", iLog 
                << "UPS=" << ups << ", "
                << "FixedUPS=" << fixedUps << ", "
                << "MaxDT=" << (longestDeltaTimeMicros*1.0e-3f) << "ms, "
                << "Capacity=" << (capacity*100.0f) << "%, " // capacity = LongestDeltaTime / FixedDeltaTime
                << "Sleep=" << (totalSleepTime/statusTimerInterval*100.0f) << "%, "
                << "Clients=" << ClientManager::GetAllCients().size() << ", "
                << "Connections=" << SocketServer::GetNumConnections()
            );
            totalSleepTimeMicros = 0;
            upsCounter = 0;
            fixedUpsCounter = 0;
            longestDeltaTimeMicros = 0;
        }

        // compute how long the current frame took
        uint64_t frameDurationMicros = Time::GetTimeMicros() - Time::currentTimeMicros;

        // wait for a short time before next loop
        if(frameDurationMicros < frameDurationTargetMicros){
            uint64_t sleepTimeMicros = frameDurationTargetMicros - frameDurationMicros;
            totalSleepTimeMicros += sleepTimeMicros;
            Time::SleepMicros(sleepTimeMicros);
        }

    }
    LogInfo("Main", "main loop end");

    // shut down robot
    robot->Shutdown();

    return true;
    
}

void Main::RequestExit(){
    LogInfo("Main", "RequestExit()");
    exitRequested = true;
}

}