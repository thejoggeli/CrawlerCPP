#pragma once

#include "Brain.h"

namespace Crawler {

class Gait;
class OnnxRunner;

class ModelBrain : public Brain {
private:

    OnnxRunner* onnxRunner = nullptr;

    std::vector<float> inputValues;
    std::vector<float> outputValues;

    std::vector<float> measuredAngles;
    std::vector<float> oldMeasuredAngles;
    std::vector<float> angleVelocities;

    std::vector<float> myAcc;
    std::vector<float> myVel;
    std::vector<float> myPos;


    void UpdateMeasuredAngles();
    void ApplyTargetAngles();
    void ApplyTargetAccelerations();
    float OutputFromTo(float val, float low, float high);

public:

    ModelBrain();
    ~ModelBrain();

    virtual void Init() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void Destroy() override;

};

}