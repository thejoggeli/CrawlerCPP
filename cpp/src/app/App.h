#pragma once

namespace Crawler {

class Robot;
class HardwareButton;
class MonoLED;

class App {
private:

    App();
    ~App();

public:

    static Robot* robot;
    static HardwareButton* mainButton;
    static MonoLED* mainButtonLED;

    static bool Init();
    static bool InitServos();
    static bool Run();
    static bool Cleanup();
    static void RequestExit(); 

};

}