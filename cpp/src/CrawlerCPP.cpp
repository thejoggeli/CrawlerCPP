#include "Eigen/Fix"
#include "Eigen/Geometry"
#include <signal.h>
#include "crawler/Main.h"
#include "core/Log.h"

// #define RUN_DEBUG
#ifdef RUN_DEBUG
    #include "debug/Debug.h"
#endif

using namespace std;
using namespace Crawler;

unsigned int sigIntCounter = 0;

Main* iMain;

void mySigIntHandler(int s){
    iMain->RequestExit();
    sigIntCounter++;
    if(sigIntCounter >= 3){
        exit(EXIT_FAILURE);
    }
}

int main(){

    iMain = new Main();

    // sigint handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = mySigIntHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // main init
    if(!iMain->Init()){
        LogError("CrawperCPP", "Main::Init() failed");
        return EXIT_FAILURE;
    }

    // main run
#ifndef RUN_DEBUG

    if(!iMain->InitServos()){
        return EXIT_FAILURE;
    }

    if(!iMain->Run()){
        LogError("CrawperCPP", "Main::Run() failed");
        return EXIT_FAILURE;
    }
#else
    Debug(iMain);
#endif

    // main cleanup
    if(!iMain->Cleanup()){
        LogError("CrawperCPP", "Main::Cleanup() failed");
        return EXIT_FAILURE;
    }

    // done
    return EXIT_SUCCESS;

}