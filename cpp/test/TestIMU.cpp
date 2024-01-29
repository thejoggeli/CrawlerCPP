#include "core/Log.h"
#include "core/Time.h"
#include <JetsonGPIO.h>
#include "parts/IMU.h"
#include "libi2c/i2c.h"
#include "comm/I2CBus.h"
#include <cmath>

using namespace Crawler;
using namespace std;

int main(){

    // create i2c bus
    I2CBus bus;
    if(bus.Open("/dev/i2c-8")){
        LogInfo("main", iLog << "bus Open() success");
    } else {
        LogError("main", iLog << "bus Open() failed");
        return EXIT_FAILURE;
    }

    // create imu
    IMU imu;
    if(!imu.Init(bus.fd)){
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

        // up vector
        float x = -acc[1];
        float y = +acc[0];
        float z = +acc[2];
        float mag = sqrtf(x*x + y*y + z*z);
        float magInv = 1.0f/mag;
        x *= magInv;
        y *= magInv;
        z *= magInv;

        // print imu values
        char buffer[10000];
        sprintf(
            buffer, 
            "dur: %5.1f ms | acc: %8.1f %8.1f %8.1f | gyro: %8.1f %8.1f %8.1f | temp: %5.1f °C | up: %6.2f %6.2f %6.2f", 
            t_total*0.001f,
            acc[0], acc[1], acc[2], 
            gyro[0], gyro[1], gyro[2], 
            temperature,
            x, y, z
        );
        LogInfo("main", buffer);

        // sleep
        Time::SleepMicros(100000);
    }

    return EXIT_SUCCESS;

}