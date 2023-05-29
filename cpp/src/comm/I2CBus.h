#pragma once

namespace Crawler {

class I2CBus {

public:

    int fd = -1;

    I2CBus();

    bool Open(const char* dev);
    void Close();

};

}