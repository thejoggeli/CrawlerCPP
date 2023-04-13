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
#include <JetsonGPIO.h>
#include <signal.h>


// #include <Eigen/Dense>

using namespace std;
using namespace Crawler;

Robot* robot;

const unsigned int mainButtonPin = 24;
HardwareButton mainButton(mainButtonPin, 200*1000);

const unsigned int mainButtonLedPin = 22;
MonoLED mainButtonLed(mainButtonLedPin);

bool exitRequested = false;

unsigned int sigIntCounter = 0;

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

    // main loop
    LogInfo("Main", "start main loop");
    while(!exitRequested){
        
        // update time
        Time::Update();

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
            lastUpdateTimeMicros = Time::currentTimeMicros;
        }

        // wait for a short time before next loop
        usleep(500);

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
    Config::Init();

    // init log levels
    LogLevels::Init();

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