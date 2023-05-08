#pragma once

#include "Brain.h"

namespace Crawler {

class CalibrationBrain : public Brain {

private:

public:

    CalibrationBrain();

    virtual void Init() override;
    virtual void Update() override;
    virtual void FixedUpdate() override;
    virtual void Destroy() override;




};


}