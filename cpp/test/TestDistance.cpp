#include "core/Log.h"
#include "parts/DistanceSensor.h"
#include "core/Time.h"
#include <JetsonGPIO.h>
#include "libi2c/i2c.h"
#include "parts/MuxI2C.h"

using namespace Crawler;

int main(){

    LogInfo("main", "GPIO init");
    GPIO::setmode(GPIO::BCM);

    // create i2c device
    int bus = i2c_open("/dev/i2c-8");
    if(bus == -1){
        LogError("Robot", iLog << "i2c_open() failed, bus=" << (int)bus);
        return EXIT_FAILURE;
    } else {
        LogInfo("Robot", iLog << "i2c_open() success, bus=" << (int)bus);
    }

    // init mux
    MuxI2C mux;
    mux.Init(bus);

    // init sensors
    std::vector<DistanceSensor*> sensors;
    for(int i = 0; i < 2; i++){
        DistanceSensor* sensor = new DistanceSensor();
        LogInfo("main", "DistanceSensor init");
        if(!sensor->Init(&mux, i)){
            LogError("Robot", iLog << "sensor " << i << " Init() failed");
            return EXIT_FAILURE;
        } else {
            LogInfo("Robot", iLog << "sensor " << i << " Init() success");
        }
        sensors.push_back(sensor);
    }

    LogInfo("main", "starting read loop");
    while(1){
        std::stringstream ss;
        for(int i = 0; i < sensors.size(); i++){
            DistanceSensor* sensor = sensors[i];
            uint16_t value = sensor->ReadProximity();
            ss << "sensor" << i << " value: " << value << " ";
        }
        LogInfo("main", ss.str());
        Time::SleepMicros(1000000);
    }

    return EXIT_SUCCESS;

}