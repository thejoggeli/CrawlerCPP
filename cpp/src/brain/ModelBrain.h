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

public:

    ModelBrain();
    ~ModelBrain();

    virtual void Init() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void Destroy() override;

};

}