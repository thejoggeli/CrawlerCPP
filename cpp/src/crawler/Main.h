#pragma once

namespace Crawler {

class Robot;
class HardwareButton;
class MonoLED;

class Main {
private:

    bool initialized = false;
    bool exitRequested = false;

public:

    Robot* robot;
    HardwareButton* mainButton;
    MonoLED* mainButtonLED;

    Main();
    ~Main();

    bool Init();
    bool InitServos();
    bool Run();
    bool Cleanup();
    void RequestExit(); 

};

}