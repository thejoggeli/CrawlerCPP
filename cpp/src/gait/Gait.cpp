#include "Gait.h"
#include "core/Log.h"
#include <cassert>

namespace Crawler {

Gait::Gait(int numLegs) : numLegs(numLegs) {
    footPositions = std::vector<Eigen::Vector3f>(numLegs, Eigen::Vector3f::Zero());
    footAngles = std::vector<float>(numLegs, 0.0f);
    bodyPosition = Eigen::Vector3f::Zero();
    bodyRotation = Eigen::Vector3f::Zero();
}

Gait::~Gait(){
    for(GaitFrame* frame : frames){
        delete frame;
    }
}

void Gait::AddFrame(const GaitFrame& frame){

    // add frame (this creates a copy)
    frames.push_back(new GaitFrame(frame));

    // set frame index
    frames[frames.size()-1]->index = frames.size()-1;

}

void Gait::AddTransition(float transitionTime){
    frames[frames.size()-1]->transitionTime = transitionTime;
    frames[frames.size()-1]->startTime = duration;
    frames[frames.size()-1]->endTime = duration + transitionTime;
    duration = frames[frames.size()-1]->endTime;
}

void Gait::StepTo(float time){

    assert(time >= 0.0f);
    assert(time <= this->duration);

    this->time = time;
    currFrameIndex = 0;
    nextFrameIndex = 1;

    // compute new frame indices
    while(time > frames[currFrameIndex]->endTime){
        currFrameIndex += 1;
        nextFrameIndex += 1;
        if(currFrameIndex == frames.size()){
            currFrameIndex = 0;
            time -= duration;
        } else if(nextFrameIndex == frames.size()){
            nextFrameIndex = 0;
        }
    }

    // compute positions
    ComputePositions();

}

void Gait::StepForward(float deltaTime){
    
    if(deltaTime == 0.0f){
        return;
    }

    // if delta time is negative, step backward
    if(deltaTime < 0.0f){
        Gait::StepBackward(-deltaTime);
        return;
    }

    // advance frame time
    time += deltaTime;

    // compute new frame indices
    while(time > frames[currFrameIndex]->endTime){
        currFrameIndex += 1;
        nextFrameIndex += 1;
        if(currFrameIndex == frames.size()){
            currFrameIndex = 0;
            time -= duration;
        } else if(nextFrameIndex == frames.size()){
            nextFrameIndex = 0;
        }
    }

    // compute positions
    ComputePositions();
    

}

void Gait::StepBackward(float deltaTime){

    if(deltaTime == 0.0f){
        return;
    }

    // if delta time is negative, step forward
    if(deltaTime < 0.0f){
        Gait::StepForward(-deltaTime);
        return;
    }


    // advance frame time
    time -= deltaTime;

    // compute new frame indices
    while(time < frames[currFrameIndex]->startTime){
        currFrameIndex -= 1;
        nextFrameIndex -= 1;
        if(currFrameIndex == -1){
            currFrameIndex = frames.size()-1;
            time += duration;
        } else if(nextFrameIndex == -1){
            nextFrameIndex = frames.size()-1;
        }
    }

    // compute positions
    ComputePositions();

}

void Gait::ComputePositions(){
    GaitFrame* a = frames[currFrameIndex];
    GaitFrame* b = frames[nextFrameIndex];
    float t = (time - a->startTime) / a->transitionTime;
    for(int i = 0; i < numLegs; i++){
        footPositions[i] = a->footPositions[i] + (b->footPositions[i] - a->footPositions[i]) * t;
        footAngles[i] = a->footAngles[i] + (b->footAngles[i] - a->footAngles[i]) * t;
    }
    bodyPosition = a->bodyPosition + (b->bodyPosition - a->bodyPosition) * t;
    bodyRotation = a->bodyRotation + (b->bodyRotation - a->bodyRotation) * t;
}

}