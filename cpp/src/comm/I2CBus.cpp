#include "I2CBus.h"
#include "libi2c/i2c.h"
#include "core/Log.h"

namespace Crawler {

I2CBus::I2CBus(){

}

bool I2CBus::Open(const char* dev){
    fd = i2c_open(dev);
    if(fd == -1){
        LogError("I2CBus", iLog << "i2c_open() failed, fd=" << fd);
        return false;
    }
    return true;
}

void I2CBus::Close(){
    if(fd != -1){
        i2c_close(fd);
    }
}

}