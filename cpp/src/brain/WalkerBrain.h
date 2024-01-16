#pragma once

#include "Brain.h"

namespace Crawler {

class Gait;

class WalkerBrain : public Brain {
private:

    float dx = 0.16f;
    float dy = 0.16f;
    float dz = 0.0f;

public:

    Gait* g_walk_1 = nullptr;
    Gait* g_walk_2 = nullptr;

    Gait* g_turn_1 = nullptr;
    Gait* g_turn_2 = nullptr;

    WalkerBrain();
    ~WalkerBrain();

    virtual void Init() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void Destroy() override;

private:

    void InitGaitWalk();
    void InitGaitTurn();

};

}