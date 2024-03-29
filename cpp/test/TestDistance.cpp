#include "core/Log.h"
#include "parts/DistanceSensor.h"
#include "core/Time.h"
#include <JetsonGPIO.h>
#include "libi2c/i2c.h"
#include "parts/MuxI2C.h"
#include "comm/I2CBus.h"

using namespace Crawler;

int main(){

    LogInfo("main", "GPIO init");
    GPIO::setmode(GPIO::BCM);
    GPIO::setup(4, GPIO::Directions::OUT);
    GPIO::setup(5, GPIO::Directions::OUT);
    GPIO::output(4, GPIO::HIGH);
    GPIO::output(5, GPIO::HIGH);

    // create i2c bus
    I2CBus bus;
    if(bus.Open("/dev/i2c-8")){
        LogInfo("main", iLog << "bus Open() success");
    } else {
        LogError("main", iLog << "bus Open() failed");
        return EXIT_FAILURE;
    }

    // init mux
    MuxI2C mux;
    mux.Init(bus.fd);

    // init sensors
    std::vector<DistanceSensor*> sensors;
    for(int i = 0; i < 4; i++){
        DistanceSensor* sensor = new DistanceSensor();
        LogInfo("main", "DistanceSensor init");
        if(!sensor->Init(&mux, i)){
            LogError("main", iLog << "sensor " << i << " Init() failed");
            return EXIT_FAILURE;
        } else {
            LogInfo("main", iLog << "sensor " << i << " Init() success");
        }
        sensors.push_back(sensor);
    } 

    LogInfo("main", "starting read loop");
    while(1){
        std::stringstream ss;
        for(int i = 0; i < sensors.size(); i++){
            DistanceSensor* sensor = sensors[i];

            uint64_t t_start = Time::GetSystemTimeMicros();
            uint16_t value = sensor->ReadProximity();
            uint64_t t_end = Time::GetSystemTimeMicros();
            uint64_t t_total = t_end - t_start;

            char buffer[2000];
            sprintf(buffer, "s%d %5.2fms %5d | ", i, t_total/1000.0f, value);
            ss << buffer;
        }
        LogInfo("main", ss.str());
        Time::SleepMicros(1000000);
    }

    return EXIT_SUCCESS;

}