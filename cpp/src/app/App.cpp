#include "App.h"

#include <JetsonGPIO.h>
#include "parts/HardwareButton.h"
#include "parts/MonoLED.h"
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
#include "remote/Packet.h"
#include "brain/SurferBrain.h"
#include "brain/GaitBrain.h"
#include "brain/EmptyBrain.h"
#include "brain/CalibrationBrain.h"
#include "ServoThread.h"
#include "PacketsComm.h"

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
    Config::ReadFile("calib");

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
        LogError("App", "serial stream open failed");
        return false;
    }

    // open servo i2c device
    if(robot->OpenI2CDevice("/dev/i2c-8")){
        LogInfo("App", "i2c device open");
    } else {
        LogError("App", "i2c device open failed");
        return false;
    }

    // init robot
    robot->Init();

    // init servo thread
    servoThread.Init(robot);

    // init info packets
    PacketsComm::Init(robot);

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
    // robot->PrintServoStatus();

    return true;
}

bool App::Cleanup(){

    LogInfo("App", "Cleanup()");
    PacketsComm::Cleanup();

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
    const uint64_t frameDurationTargetMicros = 1000000/200;
    unsigned int upsCounter = 0;
    unsigned int fixedUpsCounter = 0;
    float ups = 0.0f;
    float fixedUps = 0.0f;
    uint64_t longestFrameDurationMicros = 0;
    uint64_t longestServoThreadTimeMicros = 0;
    uint64_t totalSleepTimeMicros = 0;

    // status timer 
    unsigned int printStatusInterval = 10;
    unsigned int printStatusCounter = 0;
    Timer statusTimer;
    const float statusTimerInterval = 1.0f;

    // set fixed update rate
    Time::SetFixedDeltaTimeMicros(1000000/40);
    uint64_t lastUpdateTimeMicros = 0;

    // start up robot
    robot->Startup();
    // robot->PrintServoStatus();
    // robot->SetBrain(new SurferBrain());
    // robot->SetBrain(new GaitBrain());
    robot->SetBrain(new CalibrationBrain());
    // robot->SetBrain(new EmptyBrain());
    // robot->TorqueOff();
    // for(Leg* leg : robot->legs){
    //     leg->joints[0]->SetTargetAngle(0.0f * DEG_2_RADf);
    //     leg->joints[1]->SetTargetAngle(90.0f * DEG_2_RADf);
    //     leg->joints[2]->SetTargetAngle(90.0f * DEG_2_RADf);
    //     leg->joints[3]->SetTargetAngle(0.0f * DEG_2_RADf);
    // }

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

        // poll socket server
        SocketServer::Poll();

        // receive new packets
        ClientManager::ReceivePackets();
        
        // update clients
        ClientManager::Update();

        // update buttons
        mainButton->Update();
        if(mainButton->onPress){
            // RequestExit("MainButton");
        }

        // print debug info
        // auto& clients = ClientManager::GetAllCients();
        // if(clients.size() > 0){
        //     Client* client = clients[0].get();
        //     if(client->OnKeyDown(GamepadKey::Select)){
        //         robot->PrintServoStatus();
        //     }
        // }

        // update robot
        robot->Update();

        // fixed update
        if(Time::currentTimeMicros - lastUpdateTimeMicros >= Time::fixedDeltaTimeMicros){
            
            // wait until ServoThread signals that it finished the last loop
            // LogInfo("App", "waiting for serial comm complete");
            if(!servoThread.serialCommFinishSignal.IsSet()){
                servoThread.serialCommFinishSignal.WaitAndClear();
                ClientManager::SendLogWarning("App", iLog
                    << "serial comm in ServoThread was not complete at next FixedUpdate "
                    << "(time was " << (float)servoThread.serialCommTimeMicros*1.0e-3f << " ms)"
                );
            }

            // remember longest servo thread time
            if(servoThread.serialCommTimeMicros > longestServoThreadTimeMicros){
                longestServoThreadTimeMicros = servoThread.serialCommTimeMicros;
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
                ClientManager::SendLogWarning("App", "FixedUpdate frame drift prevention");
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
            float capacityFR = (float)longestFrameDurationMicros/(float)Time::fixedDeltaTimeMicros;
            float capacityST = (float)longestServoThreadTimeMicros/(float)Time::fixedDeltaTimeMicros;
            float totalSleepTime = (float) totalSleepTimeMicros * 1.0e-6;

            // print status to console
            printStatusCounter += 1;
            if(printStatusCounter == printStatusInterval){
                printStatusCounter = 0;
                LogInfo("App", iLog 
                    << "UPS=" << ups << ", "
                    << "FixedUPS=" << fixedUps << ", "
                    << "CapFR=" << (capacityFR*100.0f) << "%, "
                    << "CapST=" << (capacityST*100.0f) << "%, "
                    << "Sleep=" << (totalSleepTime/statusTimerInterval*100.0f) << "%, "
                    << "maxFR=" << longestFrameDurationMicros*0.001<< "ms, "
                    << "maxST=" << longestServoThreadTimeMicros*0.001 << "ms, "
                    << "Clients=" << ClientManager::GetAllCients().size()
                );
            }

            // send status to clients
            auto packet = std::make_shared<PacketMessage>("status");
            packet->AddFloat("time", Time::currentTime);
            packet->AddFloat("ups", ups);
            packet->AddFloat("fixedUps", fixedUps);
            packet->AddFloat("sleep", totalSleepTime/statusTimerInterval);
            packet->AddFloat("capFR", capacityFR);
            packet->AddFloat("capST", capacityST);
            packet->AddFloat("maxFR", longestFrameDurationMicros*0.001f);
            packet->AddFloat("maxST", longestServoThreadTimeMicros*0.001f);
            packet->AddInt("clients", ClientManager::GetAllCients().size());
            ClientManager::SendPacket(packet);

            // reset status variables
            totalSleepTimeMicros = 0;
            upsCounter = 0;
            fixedUpsCounter = 0;
            longestFrameDurationMicros = 0;
            longestServoThreadTimeMicros = 0;

        }

        // compute how long the current frame took
        uint64_t frameDurationMicros = Time::GetTimeMicros() - Time::currentTimeMicros;
        // store longest delta time
        if(frameDurationMicros > longestFrameDurationMicros){
            longestFrameDurationMicros = frameDurationMicros;
        }

        // wait for a short time before next loop
        if(frameDurationMicros < frameDurationTargetMicros){
            uint64_t sleepTimeMicros = frameDurationTargetMicros - frameDurationMicros;
            totalSleepTimeMicros += sleepTimeMicros;
            Time::SleepMicros(sleepTimeMicros);
        }

    }
    LogInfo("App", "main loop end");

    //  wait servo thread to finish
    LogInfo("App", "waiting for servo thread to finish");
    servoThread.RequestExit();
    servoThread.nextLoopSignal.Set();
    servoThread.threadFinishSignal.Wait();

    // shut down robot
    robot->Shutdown();

    return true;
    
}

void App::RequestExit(const char* requester){
    if(exitRequested){
        return;
    }

    ClientManager::SendLogInfo("App", iLog << "exit requested (requester=" << requester << ")");
    exitRequested = true;
}

}