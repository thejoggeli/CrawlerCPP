#include "App.h"

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
#include "ServoThread.h"
#include "InfoPackets.h"

using namespace std;

namespace Crawler {

static uint64_t updateCounter = 0;
static uint64_t fixedUpdateCounter = 0;
static bool initialized = false;
static bool exitRequested = false;
static ServoThread servoThread;

Robot* App::robot = nullptr;
HardwareButton* App::mainButton = nullptr;
MonoLED* App::mainButtonLED = nullptr;

App::App(){}
App::~App(){}

bool App::Init(){

    mainButton = new HardwareButton(24, 200*1000);
    mainButtonLED = new MonoLED(22); 

    if(initialized){
        return false;
    }

    LogInfo("App", "Init()");

    // init config
	if(!Config::Init()){
		LogError("App", "Config initialization failed");
		return false;
	}

    // init log levels
    if(!LogLevels::Init()){
		LogError("App", "LogLevels initialization failed");
		return false;
    }

	// init client manager
	if(!ClientManager::Init()){
		LogError("App", "ClientManager initialization failed");
		return false;
	}

	// init socket server
	if(!SocketServer::Init()){
		Log(LOG_ERROR, "App", "ServerManager initialization failed");
		return false;
	}

    // set gpio mode
    LogInfo("App", "GPIO setup");
    GPIO::setmode(GPIO::BCM);

    // init main button
    mainButton->Init();
    mainButtonLED->Init(true);

    // create robot
    LogInfo("App", "create robot");
    robot = new Robot();

    // open servo serial stream
    if(robot->OpenSerialStream("/dev/ttyTHS0")){
        LogInfo("App", "serial stream open");
    } else {
        LogInfo("App", "serial stream open failed");
        return false;
    }

    // init servo thread
    servoThread.Init(robot);

    // init info packets
    InfoPackets::Init(robot);

    // init success
    return true;
}

bool App::InitServos(){

    LogInfo("App", "InitServos()");

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
    robot->PrintServoStatus();

    return true;
}

bool App::Cleanup(){

    LogInfo("App", "Cleanup()");
    InfoPackets::Cleanup();

    // close servo serial stream
    LogInfo("App", "serial stream close");
    robot->CloseSerialStream();

    // main button cleanup
    mainButton->Cleanup();
    mainButtonLED->SetState(false);
    mainButtonLED->Cleanup();
    delete mainButton;
    delete mainButtonLED;

    return true;

}

bool App::Run(){

    LogInfo("App", "Run()");

    // frame & sleep stuff
    const uint64_t frameDurationTargetMicros = 4000;
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
    Time::SetFixedDeltaTimeMicros(1000*40);
    uint64_t lastUpdateTimeMicros = 0;

    // start up robot
    robot->Startup();
    robot->PrintServoStatus();
    // robot->SetBrain(new SurferBrain());
    robot->SetBrain(new GaitBrain());
    // robot->SetBrain(new EmptyBrain());

    // the beginning of time
    Time::Start();

    // status timer
    statusTimer.Start(statusTimerInterval);

    // start servo thread
    servoThread.nextLoopSignal.Set();
    servoThread.serialCommStartSignal.Clear();
    servoThread.serialCommFinishSignal.Clear();
    servoThread.threadFinishSignal.Clear();
    servoThread.Start();

    // main loop
    LogInfo("App", "main loop start");
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
                robot->PrintServoStatus();
            }
        }

        // update robot
        robot->Update();

        // fixed update
        if(Time::currentTimeMicros - lastUpdateTimeMicros >= Time::fixedDeltaTimeMicros){
            
            // wait until ServoThread signals that it finished the last loop
            // LogInfo("App", "waiting for serial comm complete");
            if(!servoThread.serialCommFinishSignal.IsSet()){
                LogWarning("App", iLog 
                    << "serial comm in ServoThread was not complete at next FixedUpdate "
                    << "(time was " << (float)servoThread.serialCommTimeMicros*1.0e-3f << " ms)"
                );             
                servoThread.serialCommFinishSignal.WaitAndClear();   
            }
            
            // ServoThread is waiting for next loop, meaning it is not modyfing the buffers
            // thus it is safe to apply the buffers here
            servoThread.ApplyBuffers(); 

            // signal ServoThread that it can start the next loop
            servoThread.nextLoopSignal.Set();

            // wait until ServoThread signals that it started the next loop
            servoThread.serialCommStartSignal.WaitAndClear();

            // call fixed update
            robot->FixedUpdate();

            // increase fixed fps counter
            fixedUpsCounter += 1;
            fixedUpdateCounter += 1;

            // add fixed delta time (prevents UPS drift)
            lastUpdateTimeMicros += Time::fixedDeltaTimeMicros;

            // prevent frame debt buildup if actual UPS drops below target UPS
            if(Time::currentTimeMicros - lastUpdateTimeMicros >= Time::fixedDeltaTimeMicros){
                LogWarning("App", "FixedUpdate frame drift prevention");
                lastUpdateTimeMicros = Time::currentTimeMicros;
            }

        }

        // update status
        upsCounter++;
        updateCounter += 1;
        if(statusTimer.IsFinished()){
            statusTimer.Restart(true);
            ups = (float) upsCounter / statusTimerInterval;
            fixedUps = (float) fixedUpsCounter / statusTimerInterval;
            float capacity = (float)longestDeltaTimeMicros/(float)Time::fixedDeltaTimeMicros;
            float totalSleepTime = (float) totalSleepTimeMicros * 1.0e-6;
            LogInfo("App", iLog 
                << "UPS=" << ups << ", "
                << "FixedUPS=" << fixedUps << ", "
                << "MaxDT=" << (longestDeltaTimeMicros*1.0e-3f) << "ms, "
                << "Cap=" << (capacity*100.0f) << "%, " // capacity = LongestDeltaTime / FixedDeltaTime
                << "Sleep=" << (totalSleepTime/statusTimerInterval*100.0f) << "%, "
                << "Clients=" << ClientManager::GetAllCients().size() << ", "
                << "Conn=" << SocketServer::GetNumConnections() << ", "
                << "Diff=" << (int64_t)fixedUpdateCounter - (int64_t)servoThread.loopCounter
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
    LogInfo("App", "main loop end");

    // let ServoThread finish the final loop
    // and then wait servo thread to finish
    LogInfo("App", "waiting for servo thread to finish");
    servoThread.nextLoopSignal.Set();
    servoThread.threadFinishSignal.Wait();

    // shut down robot
    robot->Shutdown();

    return true;
    
}

void App::RequestExit(){
    if(exitRequested){
        return;
    }
    LogInfo("App", "RequestExit()");
    servoThread.RequestExit();
    exitRequested = true;
}

}