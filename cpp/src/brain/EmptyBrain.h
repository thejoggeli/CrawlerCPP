#pragma once

#include "Brain.h"

namespace Crawler {

class EmptyBrain : public Brain {

private:

public:

    EmptyBrain();

    virtual void Init() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void Destroy() override;

};


}