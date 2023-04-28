#pragma once

namespace Crawler {

class Robot;

class InfoPackets {
private:
    
    InfoPackets();

public:

    static void Init(Robot* robot);
    static void Cleanup();

};


}
