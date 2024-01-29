#include "IMU.h"
#include "libi2c/i2c.h"
#include "core/Time.h"
#include "core/Log.h"
#include "ISM330DHXC/ism330dhcx_reg.h"
#include <cstring>

using namespace std;

namespace Crawler {

int32_t platform_write(void *handle, uint8_t reg, const uint8_t *buf, uint16_t len){
    IMU* imu = (IMU*) handle;
    i2c_write(imu->i2c, reg, buf, len);
    return 0;
}

int32_t platform_read(void *handle, uint8_t reg, uint8_t *buf, uint16_t len){
    IMU* imu = (IMU*) handle;
    i2c_read(imu->i2c, reg, buf, len);
    return 0;
}

IMU::IMU(){
    stmdev_ctx_t* ctx = new stmdev_ctx_t();
    ctx->write_reg = platform_write;
    ctx->read_reg = platform_read;
    ctx->handle = this;
    this->ctx = ctx;
}

IMU::~IMU(){
    if(ctx){
        delete (stmdev_ctx_t*)ctx;
    }
}

bool IMU::Init(unsigned int bus){
    
    if(i2c){
        return false;
    }

    i2c = new I2CDevice();
    i2c_init_device(i2c);
    i2c->bus = bus;
    i2c->addr = 0x6A;

    // check device id
    uint8_t deviceId;
    ism330dhcx_device_id_get((stmdev_ctx_t*)ctx, &deviceId);
    if(deviceId != ISM330DHCX_ID){
        LogError("IMU", iLog << "deviceId != ISM330DHCX_ID, deviceId is " << (int)deviceId);
        return false;
    } else {
        LogInfo("IMU", iLog << "deviceId == ISM330DHCX_ID, deviceId is " << (int)deviceId);
    }

    /* Restore default configuration */
    uint8_t rst;
    ism330dhcx_reset_set((stmdev_ctx_t*)ctx, PROPERTY_ENABLE);
    do {
        ism330dhcx_reset_get((stmdev_ctx_t*)ctx, &rst);
    } while (rst);

    /* Start device configuration. */
    ism330dhcx_device_conf_set((stmdev_ctx_t*)ctx, PROPERTY_ENABLE);
    /* Enable Block Data Update */
    ism330dhcx_block_data_update_set((stmdev_ctx_t*)ctx, PROPERTY_ENABLE);
    /* Set Output Data Rate */
    // ism330dhcx_xl_data_rate_set((stmdev_ctx_t*)ctx, ISM330DHCX_XL_ODR_12Hz5);
    // ism330dhcx_gy_data_rate_set((stmdev_ctx_t*)ctx, ISM330DHCX_GY_ODR_12Hz5);
    ism330dhcx_xl_data_rate_set((stmdev_ctx_t*)ctx, ISM330DHCX_XL_ODR_104Hz);
    ism330dhcx_gy_data_rate_set((stmdev_ctx_t*)ctx, ISM330DHCX_GY_ODR_104Hz);
    /* Set full scale */
    ism330dhcx_xl_full_scale_set((stmdev_ctx_t*)ctx, ISM330DHCX_2g);
    ism330dhcx_gy_full_scale_set((stmdev_ctx_t*)ctx, ISM330DHCX_2000dps);
    /* Configure filtering chain(No aux interface)
    *
    * Accelerometer - LPF1 + LPF2 path
    */
    ism330dhcx_xl_hp_path_on_out_set((stmdev_ctx_t*)ctx, ISM330DHCX_LP_ODR_DIV_100);
    ism330dhcx_xl_filter_lp2_set((stmdev_ctx_t*)ctx, PROPERTY_ENABLE);

    return true;
}

void IMU::ReadAcceleration(float values[3]){

    // wait until data is ready
    uint8_t reg;
    do {
        ism330dhcx_xl_flag_data_ready_get((stmdev_ctx_t*)ctx, &reg);
        Time::SleepMicros(100);
    } while(!reg);    

    /* Read acceleration field data */
    int16_t data_raw[3];
    memset(data_raw, 0x00, 3 * sizeof(int16_t));
    ism330dhcx_acceleration_raw_get((stmdev_ctx_t*)ctx, data_raw);
    values[0] = ism330dhcx_from_fs2g_to_mg(data_raw[0]);
    values[1] = ism330dhcx_from_fs2g_to_mg(data_raw[1]);
    values[2] = ism330dhcx_from_fs2g_to_mg(data_raw[2]);

}

void IMU::ReadGyro(float values[3]){

    // wait until data is ready
    uint8_t reg;
    do {
        ism330dhcx_gy_flag_data_ready_get((stmdev_ctx_t*)ctx, &reg);
        Time::SleepMicros(100);
    } while(!reg);

    /* Read acceleration field data */
    int16_t data_raw[3];
    memset(data_raw, 0x00, 3 * sizeof(int16_t));
    ism330dhcx_angular_rate_raw_get((stmdev_ctx_t*)ctx, data_raw);
    values[0] = ism330dhcx_from_fs2000dps_to_mdps(data_raw[0]);
    values[1] = ism330dhcx_from_fs2000dps_to_mdps(data_raw[1]);
    values[2] = ism330dhcx_from_fs2000dps_to_mdps(data_raw[2]);

}

float IMU::ReadTemperature(){

    // wait until data is ready
    uint8_t reg;
    do {
        ism330dhcx_temp_flag_data_ready_get((stmdev_ctx_t*)ctx, &reg);
        Time::SleepMicros(100);
    } while(!reg);

    /* Read temperature data */
    int16_t data_raw_temperature;
    memset(&data_raw_temperature, 0x00, sizeof(int16_t));
    ism330dhcx_temperature_raw_get((stmdev_ctx_t*)ctx, &data_raw_temperature);
    return ism330dhcx_from_lsb_to_celsius(data_raw_temperature);

}

}