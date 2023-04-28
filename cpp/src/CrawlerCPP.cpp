#include "Eigen/Fix"
#include "Eigen/Geometry"
#include <signal.h>
#include "app/App.h"
#include "core/Log.h"

// #define RUN_DEBUG
#ifdef RUN_DEBUG
    #include "debug/Debug.h"
#endif

using namespace std;
using namespace Crawler;

unsigned int sigIntCounter = 0;

void mySigIntHandler(int s){
    App::RequestExit();
    sigIntCounter++;
    if(sigIntCounter >= 3){
        exit(EXIT_FAILURE);
    }
}

int main(){

    // sigint handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = mySigIntHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // main init
    if(!App::Init()){
        LogError("CrawperCPP", "App::Init() failed");
        return EXIT_FAILURE;
    }

    // main run
#ifndef RUN_DEBUG

    if(!App::InitServos()){
        LogError("CrawperCPP", "App::InitServos() failed");
        // return EXIT_FAILURE;
    }

    if(!App::Run()){
        LogError("CrawperCPP", "App::Run() failed");
        return EXIT_FAILURE;
    }
#else
    Debug(iApp);
#endif

    // main cleanup
    if(!App::Cleanup()){
        LogError("CrawperCPP", "App::Cleanup() failed");
        return EXIT_FAILURE;
    }

    // done
    return EXIT_SUCCESS;

}