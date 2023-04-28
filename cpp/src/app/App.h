#pragma once

namespace Crawler {

class Robot;
class HardwareButton;
class MonoLED;

class App {
public:

    Robot* robot;
    HardwareButton* mainButton;
    MonoLED* mainButtonLED;

    App();
    ~App();

    bool Init();
    bool InitServos();
    bool Run();
    bool Cleanup();
    void RequestExit(); 

};

}