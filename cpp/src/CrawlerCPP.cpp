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

App* app;

void mySigIntHandler(int s){
    app->RequestExit();
    sigIntCounter++;
    if(sigIntCounter >= 3){
        exit(EXIT_FAILURE);
    }
}

int main(){

    app = new App();

    // sigint handler
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = mySigIntHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // main init
    if(!app->Init()){
        LogError("CrawperCPP", "App::Init() failed");
        return EXIT_FAILURE;
    }

    // main run
#ifndef RUN_DEBUG

    if(!app->InitServos()){
        LogError("CrawperCPP", "App::InitServos() failed");
        // return EXIT_FAILURE;
    }

    if(!app->Run()){
        LogError("CrawperCPP", "App::Run() failed");
        return EXIT_FAILURE;
    }
#else
    Debug(iApp);
#endif

    // main cleanup
    if(!app->Cleanup()){
        LogError("CrawperCPP", "App::Cleanup() failed");
        return EXIT_FAILURE;
    }

    // done
    return EXIT_SUCCESS;

}