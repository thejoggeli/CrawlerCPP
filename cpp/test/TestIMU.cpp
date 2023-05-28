#include "core/Log.h"
#include "core/Time.h"
#include <JetsonGPIO.h>
#include "parts/IMU.h"
#include "libi2c/i2c.h"

using namespace Crawler;

int main(){

    // create i2c device
    int bus = i2c_open("/dev/i2c-8");
    if(bus == -1){
        LogError("main", iLog << "i2c_open() failed, bus=" << (int)bus);
        return EXIT_FAILURE;
    } else {
        LogInfo("main", iLog << "i2c_open() success, bus=" << (int)bus);
    }

    // create imu
    IMU imu;
    if(!imu.Init(bus)){
        LogError("main", iLog << "IMU Init() failed");
        return EXIT_FAILURE;
    }

    LogInfo("main", "starting read loop");
    while(1){

        uint64_t t_start = Time::GetSystemTimeMicros();

        // read acceleration
        float acc[3];
        imu.ReadAcceleration(acc);

        // read gyro
        float gyro[3];
        imu.ReadGyro(gyro);

        // read temperature
        float temperature;
        temperature = imu.ReadTemperature();

        // measure time
        uint64_t t_end = Time::GetSystemTimeMicros();
        uint64_t t_total = t_end - t_start;

        // print imu values
        char buffer[10000];
        sprintf(
            buffer, 
            "dur: %6.2f ms | acc: %10.3f %10.3f %10.3f | gyro: %10.3f %10.3f %10.3f | temp: %6.2f °C", 
            t_total*0.001f,
            acc[0], acc[1], acc[2], 
            gyro[0], gyro[1], gyro[2], 
            temperature
        );
        LogInfo("main", buffer);

        // sleep
        Time::SleepMicros(1000000);
    }

    return EXIT_SUCCESS;

}