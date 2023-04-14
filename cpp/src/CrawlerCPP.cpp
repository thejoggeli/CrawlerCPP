#include "Eigen/Fix"
#include "Eigen/Geometry"

#include "comm/SerialStream.h"
#include "parts/XYZServo.h"
#include "robot/Leg.h"
#include "robot/Robot.h"
#include "brain/SurferBrain.h"
#include "brain/EmptyBrain.h"

#include "core/Time.h"
#include "core/Config.h"
#include "core/Log.h"

#include <cmath>
#include "math/Mathf.h"
#include <cassert>
#include "util/HardwareButton.h"
#include "util/MonoLED.h"
#include "util/Timer.h"
#include <JetsonGPIO.h>
#include <signal.h>

#include "remote/SocketServer.h"
#include "remote/ClientManager.h"
#include "remote/Client.h"
#include "events/EventManager.h"

// #include <Eigen/Dense>

using namespace std;
using namespace Crawler;

Robot* robot;

const unsigned int mainButtonPin = 24;
HardwareButton mainButton(mainButtonPin, 200*1000);

const unsigned int mainButtonLedPin = 22;
MonoLED mainButtonLed(mainButtonLedPin);

bool exitRequested = false;

const uint64_t frameDurationTargetMicros = 1000;

unsigned int sigIntCounter = 0;

unsigned int upsCounter = 0;
unsigned int fixedUpsCounter = 0;
float ups = 0.0f;
float fixedUps = 0.0f;
const float statusTimerInterval = 10.0f;
Timer statusTimer;
uint64_t longestDeltaTimeMicros = 0;
uint64_t totalSleepTimeMicros = 0;

void mySigIntHandler(int s){
    exitRequested = true;
    sigIntCounter++;
    if(sigIntCounter >= 3){
        exit(EXIT_FAILURE);
    }
}

EmptyBrain* createEmptyBrain(Robot* robot){
    EmptyBrain* brain = new EmptyBrain();
    robot->SetBrain(brain);
    return brain;
}

SurferBrain* createSurferBrain(Robot* robot){

    SurferBrain* brain = new SurferBrain();
    robot->SetBrain(brain); // pass ownership of brain to robot

    // surfer stance
    float hipAngle = 0.0f * DEG_2_RADf;
    float feetXY = 0.12f;
    float feetZ = -0.15f;
    float feetPhi = 0.0 * DEG_2_RADf;
    brain->SetStance(feetXY, feetZ, hipAngle, feetPhi);

    // Eigen::Vector3f pivot = robot->legs[3]->hipTranslation.translation();
    // pivot[2] -= 0.04f; 
    // Eigen::Vector3f pivot = robot->legs[3]->hipTranslation.translation();
    // brain->SetPivot(pivot);
    
    return brain;
}

void run(){

    LogInfo("Main", "run()");

    // start up robot
    robot->Startup();

    // debug
    robot->legs[1]->joints[3]->SetTargetAngle(0.0f * DEG_2_RADf);

    // create robot brain
    SurferBrain* brain = createSurferBrain(robot);

    // set fixed update rate
    Time::SetFixedDeltaTimeMicros(1000*25);
    uint64_t lastUpdateTimeMicros = 0;

    // the beginning of time
    Time::Start();

    // status timer
    statusTimer.Start(statusTimerInterval);

    // main loop
    LogInfo("Main", "start main loop");
    while(!exitRequested){
        
        // update time
        Time::Update();

        // comput
        if(Time::deltaTimeMicros > longestDeltaTimeMicros){
            longestDeltaTimeMicros = Time::deltaTimeMicros;
        }

        // poll socket server
        SocketServer::Poll();

        // clear events from last tick
        EventManager::PopNewEvents();
        
        // update clients
        ClientManager::Update();

        // fire events
        EventManager::Update();

        // update buttons
        mainButton.Update();
        if(mainButton.onPress){
            exitRequested = true;
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
                << "Connections=" << SocketServer::GetNumConnections() << ", "
                << "Events=" << EventManager::eventCounterTemp << "/" << EventManager::eventCounterTotal
                << ""
            );
            totalSleepTimeMicros = 0;
		    EventManager::eventCounterTemp = 0;
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
            usleep(sleepTimeMicros);
        }

    }

    // shut down robot
    robot->Shutdown();

}

void runDebug(){

    LogInfo("Main", "runDebug()");

    robot->RebootServos(3.5f);
    robot->legs[1]->joints[3]->servo->setPosition(511, 100);
    Time::Sleep(1.5f);

    Time::Start();

    LogInfo("Main", "start main loop");
    while(!exitRequested){
        Time::Update();
        mainButton.Update();
        if(mainButton.onPress){
            Log(iLog << "mainBUtton.onPress");
            mainButtonLed.ToggleState();
        }
        Time::SleepMicros(500);
    }
}

int main(){

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
		Log(LOG_ERROR, "Ledlib", "ServerManager initialization failed");
		return false;
	}

    // sigint handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = mySigIntHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // set gpio mode
    LogInfo("Main", "GPIO setup");
    GPIO::setmode(GPIO::BCM);

    // init buttons
    mainButton.Init();

    // init leds
    mainButtonLed.Init(true);

    // create robot
    LogInfo("Main", "create robot");
    robot = new Robot();

    // open servo serial stream
    if(robot->OpenSerialStream("/dev/ttyTHS0")){
        LogInfo("Main", "serial stream open");
    } else {
        LogInfo("Main", "serial stream open failed");
        return EXIT_FAILURE;
    }

    // start program
    // runDebug();
    run();

    // close servo serial stream
    LogInfo("Main", "serial stream close");
    robot->CloseSerialStream();

    // cleanup
    LogInfo("Main", "cleanup");
    mainButton.Cleanup();
    mainButtonLed.Cleanup();

    // done
    return EXIT_SUCCESS;

}