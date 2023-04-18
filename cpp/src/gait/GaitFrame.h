#pragma once

#include "Eigen/Fix"
#include "Eigen/Geometry"
#include <vector>

namespace Crawler {

class GaitFrame {
public:

    int numLegs;

    // these values are initialized with Gait::AddFrame() and Gait::AddTransition()
    int index = 0;
    float startTime = 0.0f;
    float endTime = 0.0f;
    float transitionTime = 0.0f;

    // these value must be initialized before adding them with Gait::AddFrame()
    std::vector<Eigen::Vector3f> footPositions; // default all {0, 0, 0}
    std::vector<float> footAngles; // default all 0
    Eigen::Vector3f bodyPosition; // default {0, 0, 0}
    Eigen::Vector3f bodyRotation; // default {0, 0, 0}

    GaitFrame(int numLegs = 4);
    GaitFrame(const GaitFrame& frame);

    static void Interpolate(GaitFrame& out, const GaitFrame& a, const GaitFrame& b, float t);

};

}