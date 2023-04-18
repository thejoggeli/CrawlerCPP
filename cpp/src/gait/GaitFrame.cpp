#include "GaitFrame.h"
#include "core/Log.h"

namespace Crawler {

GaitFrame::GaitFrame(int numLegs) : numLegs(numLegs) {
    footPositions = std::vector<Eigen::Vector3f>(numLegs, Eigen::Vector3f::Zero());
    footAngles = std::vector<float>(numLegs, 0.0f);
    bodyPosition = Eigen::Vector3f::Zero();
    bodyRotation = Eigen::Vector3f::Zero();
}

GaitFrame::GaitFrame(const GaitFrame& frame) {
    transitionTime = frame.transitionTime;
    numLegs = frame.numLegs;
    footPositions = frame.footPositions;
    footAngles = frame.footAngles;
    bodyPosition = frame.bodyPosition;
    bodyRotation = frame.bodyRotation;
    LogDebug("GaitFrame", "CopyConstructor");
}

void GaitFrame::Interpolate(GaitFrame& out, const GaitFrame& a, const GaitFrame& b, float t){
    for(int i = 0; i < out.numLegs; i++){
        out.footPositions[i] = a.footPositions[i] + (b.footPositions[i] - a.footPositions[i]) * t;
        out.footAngles[i] = a.footAngles[i] + (b.footAngles[i] - a.footAngles[i]) * t;
    }
    out.bodyPosition = a.bodyPosition + (b.bodyPosition - a.bodyPosition) * t;
    out.bodyRotation = a.bodyRotation + (b.bodyRotation - a.bodyRotation) * t;
}

}