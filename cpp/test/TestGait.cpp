#include "core/Log.h"
#include "gait/Gait.h"
#include "gait/GaitFrame.h"

using namespace Crawler;

void printGait(const Gait& gait){

    auto& p = gait.bodyPosition;

    LogInfo("TestGait", iLog 
        << "time=" << gait.time << " / "
        << "currFrameIndex=" << gait.currFrameIndex << " / "
        << "nextFrameIndex=" << gait.nextFrameIndex << " / "
        << "bodyPosition=(" << p.x() << ", " << p.y() << ", " << p.z() << ")" 
    );
}

int main(){

    int numLegs = 1;
    Gait gait = Gait(numLegs);

    GaitFrame frame(numLegs);

    frame.bodyPosition = {0.0f, 0.0, 0.0f};
    gait.AddFrame(frame);
    gait.AddTransition(1.0f);

    frame.bodyPosition = {1.0f, 1.0, 1.0f};
    gait.AddFrame(frame);
    gait.AddTransition(2.0f);

    frame.bodyPosition = {5.0f, 5.0, 5.0f};
    gait.AddFrame(frame);
    gait.AddTransition(1.0f);

    frame.bodyPosition = {1.0f, 2.0, 3.0f};
    gait.AddFrame(frame);
    gait.AddTransition(1.0f);

    LogInfo("TestGait", iLog << "duration=" << gait.duration);
    for(GaitFrame* frame : gait.frames){
        LogInfo("TestGait", iLog 
            << "index=" << frame->index << " / "
            << "startTime=" << frame->startTime << " / "
            << "endTime=" << frame->endTime << " / "
            << "transitionTime=" << frame->transitionTime
        );
    }

    printGait(gait);
    for(int i = 0; i < 75; i++){
        gait.StepForward(0.1f);
        printGait(gait);
    }

    for(int i = 0; i < 75; i++){
        gait.StepBackward(0.1f);
        printGait(gait);
    }
    

    return EXIT_SUCCESS;
}