#include "core/Log.h"
#include "parts/WeightSensor.h"
#include "core/Time.h"
#include <JetsonGPIO.h>

using namespace Crawler;

int main(){

    unsigned int dataPin = 21;
    unsigned int clockPin = 17;

    LogInfo("main", "GPIO init");
    GPIO::setmode(GPIO::BCM);

    // weight sensor
    WeightSensor weightSensor;
    LogInfo("main", "weightSensor init");
    if(!weightSensor.Init(dataPin, clockPin)){
        LogError("Robot", iLog << "weightSensor.Init() failed");
    } else {
        LogInfo("Robot", iLog << "weightSensor.Init() success");
    }

    LogInfo("main", "starting read loop");
    while(1){
        float value = weightSensor.GetValue();
        LogInfo("main", iLog << "value: " << value);
        Time::SleepMicros(1000000);
    }

    return EXIT_SUCCESS;

}