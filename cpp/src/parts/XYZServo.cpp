#include "XYZServo.h"
#include <iostream>

#define CMD_EEPROM_WRITE 0x01
#define CMD_EEPROM_READ 0x02
#define CMD_RAM_WRITE 0x03
#define CMD_RAM_READ 0x04
#define CMD_I_JOG 0x05
#define CMD_S_JOG 0x06
#define CMD_STAT 0x07
#define CMD_ROLLBACK 0x08
#define CMD_REBOOT 0x09

#define SET_POSITION_CONTROL 0
#define SET_SPEED_CONTROL 1
#define SET_TORQUE_OFF 2
#define SET_POSITION_CONTROL_SERVO_ON 3

static uint8_t jog_data_buffer[5*256];

XYZServo::XYZServo(SerialStream *stream, uint8_t id) {
    this->stream = stream;
    this->id = id;
    this->lastError = XYZServoError::None;
}

void XYZServo::eepromWrite(uint8_t startAddress, const uint8_t *data, uint8_t dataSize) { memoryWrite(CMD_EEPROM_WRITE, startAddress, data, dataSize); }

void XYZServo::eepromRead(uint8_t startAddress, uint8_t *data, uint8_t dataSize) { memoryRead(CMD_EEPROM_READ, startAddress, data, dataSize); }

void XYZServo::ramWrite(uint8_t startAddress, const uint8_t *data, uint8_t dataSize) { memoryWrite(CMD_RAM_WRITE, startAddress, data, dataSize); }

void XYZServo::ramRead(uint8_t startAddress, uint8_t *data, uint8_t dataSize) { memoryRead(CMD_RAM_READ, startAddress, data, dataSize); }

void XYZServo::writeBaudRateEeprom(XYZServoBaudRate baud) {
    uint8_t b = (uint8_t)baud;
    memoryWrite(CMD_EEPROM_WRITE, 5, &b, 1);
}

XYZServoBaudRate XYZServo::readBaudRateEeprom() {
    uint8_t b = 0;
    memoryRead(CMD_EEPROM_READ, 5, &b, 1);
    return (XYZServoBaudRate)b;
}

void XYZServo::writeIdEeprom(uint8_t id) { memoryWrite(CMD_EEPROM_WRITE, 6, &id, 1); }

uint8_t XYZServo::readIdEeprom() {
    uint8_t id = 0;
    memoryRead(CMD_EEPROM_READ, 6, &id, 1);
    return id;
}

void XYZServo::writeIdRam(uint8_t id) { memoryWrite(CMD_RAM_WRITE, 0, &id, 1); }

void XYZServo::writeAckPolicyEeprom(XYZServoAckPolicy policy) {
    uint8_t p = (uint8_t)policy;
    eepromWrite(7, &p, 1);
}

XYZServoAckPolicy XYZServo::readAckPolicyEeprom() {
    uint8_t result = 0;
    eepromRead(7, &result, 1);
    return (XYZServoAckPolicy)result;
}

void XYZServo::writeAckPolicyRam(XYZServoAckPolicy policy) {
    uint8_t p = (uint8_t)policy;
    ramWrite(1, &p, 1);
}

void XYZServo::writeAlarmLedPolicyRam(uint8_t policy) { ramWrite(2, &policy, 1); }

void XYZServo::writeSpdctrlPolicyRam(XYZServoSpdctrlPolicy policy) {
    uint8_t p = (uint8_t)policy;
    ramWrite(4, &p, 1);
}

void XYZServo::writeMaxPwmRam(uint16_t value) { ramWrite(16, (uint8_t *)&value, 2); }

void XYZServo::writeLedControl(uint8_t control) { ramWrite(53, &control, 1); }

XYZServoAckPolicy XYZServo::readAckPolicyRam() {
    uint8_t result = 0;
    ramRead(1, &result, 1);
    return (XYZServoAckPolicy)result;
}

XYZServoStatus XYZServo::readStatus() {

    XYZServoStatus status;

    if(!flushRead()){
        status.iBus = 0;
        status.position = 0;
        status.posRef = 0;
        status.pwm = 0;
        status.statusDetail = 0;
        status.statusError = 0;
        return status;
    }

    sendRequest(CMD_STAT, nullptr, 0);
    readAck(CMD_STAT, (uint8_t *)&status, 10);
    return status;
}

void XYZServo::setPosition(const uint16_t position, uint8_t playtime) { 
    jog_data_buffer[0] = position & 0xFF;
    jog_data_buffer[1] = position >> 8 & 0xFF;
    jog_data_buffer[2] = SET_POSITION_CONTROL;
    jog_data_buffer[3] = id;
    jog_data_buffer[4] = playtime;
    sendRequest(CMD_I_JOG, jog_data_buffer, 5);
}

void XYZServo::setPositions(const uint16_t positions[], const uint8_t ids[], const uint8_t playtimes[], uint8_t num_servos){
    int data_ptr = 0;
    for(int i = 0; i < num_servos; i++){
        jog_data_buffer[data_ptr++] = positions[i] & 0xFF;
        jog_data_buffer[data_ptr++] = positions[i] >> 8 & 0xFF;
        jog_data_buffer[data_ptr++] = SET_POSITION_CONTROL;
        jog_data_buffer[data_ptr++] = ids[i];
        jog_data_buffer[data_ptr++] = playtimes[i];
    }
    sendRequest(CMD_I_JOG, jog_data_buffer, num_servos*5);
}

void XYZServo::setPositionsSync(const uint16_t positions[], const uint8_t ids[], uint8_t playtime, uint8_t num_servos){    
    jog_data_buffer[0] = playtime;
    int data_ptr = 1;
    for(int i = 0; i < num_servos; i++){
        jog_data_buffer[data_ptr++] = positions[i] & 0xFF;
        jog_data_buffer[data_ptr++] = positions[i] >> 8 & 0xFF;
        jog_data_buffer[data_ptr++] = SET_POSITION_CONTROL;
        jog_data_buffer[data_ptr++] = ids[i];
    }
    sendRequest(CMD_S_JOG, jog_data_buffer, num_servos*4+1);
}

void XYZServo::setSpeed(int16_t speed, uint8_t playtime) { 
    jog_data_buffer[0] = speed & 0xFF;
    jog_data_buffer[1] = speed >> 8 & 0xFF;
    jog_data_buffer[2] = SET_SPEED_CONTROL;
    jog_data_buffer[3] = id;
    jog_data_buffer[4] = playtime;
    sendRequest(CMD_I_JOG, jog_data_buffer, 5);
}

void XYZServo::torqueOff() { 
    jog_data_buffer[0] = 0;
    jog_data_buffer[1] = 0;
    jog_data_buffer[2] = SET_TORQUE_OFF;
    jog_data_buffer[3] = id;
    jog_data_buffer[4] = 0;
    sendRequest(CMD_I_JOG, jog_data_buffer, 5);
}

void XYZServo::rollback() { sendRequest(CMD_ROLLBACK, nullptr, 0); }

void XYZServo::reboot() { sendRequest(CMD_REBOOT, nullptr, 0); }

int XYZServo::flushRead() {
    while (stream->available()) {
        int ret = stream->read(flushReadTimeout);
        if(ret != 1){
            switch(ret){
                case -2: {
                    lastError = XYZServoError::FlushReadErrorWhileReadingByte;
                    break;
                }
                case -1: {
                    lastError = XYZServoError::FlushReadErrorWhileSettingTimeout;
                    break;
                }
                case 0: {
                    lastError = XYZServoError::FlushReadTimeout;
                    break;
                }
                default: {
                    lastError = XYZServoError::FlushError;
                    break;
                }
            }
            return 0;
        }
    }
    return 1;
    // return stream->flushRead();
}

void XYZServo::sendRequest(uint8_t cmd, const uint8_t *data1, uint8_t data1Size, const uint8_t *data2, uint8_t data2Size) {
    uint8_t header[7];

    uint8_t size = data1Size + data2Size + sizeof(header);

    uint8_t checksum = size ^ id ^ cmd;
    for (uint8_t i = 0; i < data1Size; i++) {
        checksum ^= data1[i];
    }
    for (uint8_t i = 0; i < data2Size; i++) {
        checksum ^= data2[i];
    }

    header[0] = 0xFF;
    header[1] = 0xFF;
    header[2] = size;
    header[3] = id;
    header[4] = cmd;
    header[5] = checksum & 0xFE;
    header[6] = ~checksum & 0xFE;

    stream->write(header, sizeof(header));
    if (data1Size) {
        stream->write(data1, data1Size);
    }
    if (data2Size) {
        stream->write(data2, data2Size);
    }

    lastError = XYZServoError::None;
}

void XYZServo::readAck(uint8_t cmd, uint8_t *data1, uint8_t data1Size, uint8_t *data2, uint8_t data2Size) {
    // The CMD byte for an acknowledgment always has bit 6 set.
    cmd |= 0x40;

    uint8_t header[7];

    uint8_t size = sizeof(header) + data1Size + data2Size;

    uint8_t byteCount = stream->readBytes(header, sizeof(header), readAckHeaderTimeout);
    if (byteCount != sizeof(header)) {
        if(byteCount == -1){
            lastError = XYZServoError::HeaderErrorWhileSettingTimeout;
            return;
        } else if(byteCount == -2){
            lastError = XYZServoError::HeaderErrorWhileReadingByte;
            return;
        } else if (byteCount != data1Size) {
            lastError = XYZServoError::HeaderTimeout;
            return;
        }
    }

    if (header[0] != 0xFF) {
        lastError = XYZServoError::HeaderByte1Wrong;
        return;
    }

    if (header[1] != 0xFF) {
        lastError = XYZServoError::HeaderByte2Wrong;
        return;
    }

    if (header[3] != id) {
        lastError = XYZServoError::IdWrong;
        return;
    }

    if (header[4] != cmd) {
        lastError = XYZServoError::CmdWrong;
        return;
    }

    if (header[2] != size) {
        lastError = XYZServoError::SizeWrong;
        return;
    }

    if (data1Size) {
        byteCount = stream->readBytes(data1, data1Size, readAckData1Timeout);
        if(byteCount == -1){
            lastError = XYZServoError::Data1ErrorWhileSettingTimeout;
            return;
        } else if(byteCount == -2){
            lastError = XYZServoError::Data1ErrorWhileReadingByte;
            return;
        } else if (byteCount != data1Size) {
            lastError = XYZServoError::Data1Timeout;
            return;
        }
    }

    if (data2Size) {
        byteCount = stream->readBytes(data2, data2Size, readAckData2Timeout);
        if(byteCount == -1){
            lastError = XYZServoError::Data2ErrorWhileSettingTimeout;
            return;
        } else if(byteCount == -2){
            lastError = XYZServoError::Data2ErrorWhileReadingByte;
            return;
        } else if (byteCount != data2Size) {
            lastError = XYZServoError::Data2Timeout;
            return;
        }
    }

    uint8_t checksum = size ^ id ^ cmd;
    for (uint8_t i = 0; i < data1Size; i++) {
        checksum ^= data1[i];
    }
    for (uint8_t i = 0; i < data2Size; i++) {
        checksum ^= data2[i];
    }

    if (header[5] != (checksum & 0xFE)) {
        lastError = XYZServoError::Checksum1Wrong;
        return;
    }

    if (header[6] != (~checksum & 0xFE)) {
        lastError = XYZServoError::Checksum2Wrong;
        return;
    }

    lastError = XYZServoError::None;
}

void XYZServo::memoryWrite(uint8_t cmd, uint8_t startAddress, const uint8_t *data, uint8_t dataSize) {
    uint8_t request[2];
    request[0] = startAddress;
    request[1] = dataSize;

    sendRequest(cmd, request, sizeof(request), data, dataSize);
}

void XYZServo::memoryRead(uint8_t cmd, uint8_t startAddress, uint8_t *data, uint8_t dataSize) {
    if(!flushRead()){
        return;
    }

    uint8_t request[2];
    request[0] = startAddress;
    request[1] = dataSize;
    sendRequest(cmd, request, sizeof(request));

    uint8_t response[4];
    readAck(cmd, response, 4, data, dataSize);
    if (getLastError()) {
        return;
    }

    // Despite what the A1-16 datasheet says, the first two bytes of the response
    // tend to 0, and the start address and data size come after that.

    if (response[2] != request[0]) {
        lastError = XYZServoError::ReadOffsetWrong;
        return;
    }

    if (response[3] != request[1]) {
        lastError = XYZServoError::ReadLengthWrong;
        return;
    }
}