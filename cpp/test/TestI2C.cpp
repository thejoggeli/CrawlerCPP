#include "libi2c/i2c.h"
#include "core/Log.h"
#include "parts/DistanceSensor.h"
#include "core/Time.h"

using namespace Crawler;

int main(){

    // open i2c device
    int bus = i2c_open("/dev/i2c-8");
    if(bus == -1){
        LogError("main", iLog << "i2c_open() failed, bus=" << (int)bus);
        return EXIT_FAILURE;
        return false;
    } else {
        LogInfo("main", iLog << "i2c_open() success, bus=" << (int)bus);
    }

    // create i2c device
    I2CDevice dev = I2CDevice();
    dev.bus = bus;
    dev.addr = 0x13;
    dev.iaddr_bytes = 1;
    dev.page_bytes = 16;
    dev.tenbit = false;

    // distance sensor
    DistanceSensor distanceSensor;
    LogInfo("main", "distanceSensor init");
    distanceSensor.Init(&dev);

    LogInfo("main", "starting read loop");
    while(1){
        uint16_t proximity = distanceSensor.ReadProximity();
        LogInfo("main", iLog << "proximity: " << proximity);
        Time::SleepMicros(1000000);
    }

    return EXIT_SUCCESS;

}