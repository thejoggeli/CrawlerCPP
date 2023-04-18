#pragma once

#include "Brain.h"

namespace Crawler {

class Gait;

class GaitBrain : public Brain {
private:

public:

    bool playing = true;
    float speed = 1.0f;
    Gait* gait = nullptr;

    GaitBrain();
    ~GaitBrain();

    virtual void Init() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void Destroy() override;

};

}