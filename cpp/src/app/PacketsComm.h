#pragma once

namespace Crawler {

class Robot;

class PacketsComm {
private:
    
    PacketsComm();

public:

    static void Init(Robot* robot);
    static void Cleanup();

};


}
