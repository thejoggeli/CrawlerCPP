#pragma once

#include <vector>
#include "GaitFrame.h"

namespace Crawler {

class Gait {
private:


public: 

    int numLegs;

    float duration = 0.0f;
    float time = 0.0f;
    int currFrameIndex = 0;
    int nextFrameIndex = 1;
    std::vector<GaitFrame*> frames;
    
    std::vector<Eigen::Vector3f> footPositions;
    std::vector<float> footAngles;
    Eigen::Vector3f bodyPosition;
    Eigen::Vector3f bodyRotation;

    Gait(int numLegs = 4);
    ~Gait();

    void AddFrame(const GaitFrame& frame);
    void AddTransition(float transitionTime);

    void StepTo(float time);
    void StepForward(float deltaTime);
    void StepBackward(float deltaTime);

    void ComputePositions();

};

}