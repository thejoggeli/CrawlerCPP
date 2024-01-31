#pragma once

#include <string>
#include "PacketType.h"
#include "buffer/ByteBufferWriter.h"
#include "buffer/ByteBufferReader.h"
#include "buffer/SmartBuffer.h"
#include <vector>
#include <unordered_map>
#include "GamepadKeys.h"

namespace Crawler {

class Packet {

private:

    static uint32_t nextPacketId;

public:

    int errors = 0;
    int clientId = -1;
    uint32_t id;

    PacketType type = PacketType::Invalid;

    Packet(PacketType type);

    static std::shared_ptr<Packet> Unpack(const uint8_t* bytes, unsigned int size, int clientId = -1);
    void Pack(std::vector<uint8_t>& buffer);
    virtual void UnpackInner(ByteBufferReader& reader);
    virtual void PackInner(ByteBufferWriter& writer);

};

struct PacketMessage : public Packet {
    std::string message;
    std::unordered_map<std::string, std::string> params;
    PacketMessage();
    PacketMessage(const char* message);
    void PackInner(ByteBufferWriter& writer) override;
    void UnpackInner(ByteBufferReader& reader) override;
    void SetMessage(const char* message);
    void AddFloat(const std::string& key, float value);
    void AddInt(const std::string& key, int value);
    void AddBool(const std::string& key, bool value);
    void AddString(const std::string& key, const char* string);
    float GetFloat(const std::string& key);
    int GetInt(const std::string& key);
    bool GetBool(const std::string& key);
    const char* GetString(const std::string& key);
};

struct PacketGamepadKey : public Packet {
    GamepadKey key;
    GamepadKeyState state;
    PacketGamepadKey();
    void PackInner(ByteBufferWriter& writer) override;
    void UnpackInner(ByteBufferReader& reader) override;
};

struct PacketGamepadJoystick : public Packet {
    GamepadKey key;
    float x, y;
    PacketGamepadJoystick();
    void PackInner(ByteBufferWriter& writer) override;
    void UnpackInner(ByteBufferReader& reader) override;
};

struct PacketRequestSetJointPosition : public Packet {
    uint8_t leg, joint;
    float x, y, z;
    PacketRequestSetJointPosition();
    void UnpackInner(ByteBufferReader& reader) override;
};

struct PacketRequestSetLegAngles : public Packet {
    uint8_t leg;
    std::vector<float> angles;
    PacketRequestSetLegAngles();
    void UnpackInner(ByteBufferReader& reader) override;
};

struct PacketRequestLegData : public Packet {
    std::vector<uint8_t> legIds;
    PacketRequestLegData();
    void UnpackInner(ByteBufferReader& reader) override;
};

struct PacketRespondLegData : public Packet {
    std::vector<uint8_t> legIds;
    std::vector<float> targetAngle;
    std::vector<float> measuredAngle;
    std::vector<float> temperature;
    std::vector<float> current;
    std::vector<float> voltage;
    std::vector<float> pwm;
    std::vector<uint8_t> statusDetail;
    std::vector<uint8_t> statusError;
    std::vector<float> distance;
    std::vector<float> weight;  
    PacketRespondLegData();
    void PackInner(ByteBufferWriter& writer) override;
};

struct PacketRequestLegAngles : public Packet {
    std::vector<uint8_t> legIds;
    uint8_t flags;
    PacketRequestLegAngles();
    void UnpackInner(ByteBufferReader& reader) override;
};

struct PacketRespondLegAngles : public Packet {
    std::vector<uint8_t> legIds;
    uint8_t flags;
    std::vector<float> targetAngle;
    std::vector<float> measuredAngle;
    std::vector<uint16_t> targetXYZ;
    std::vector<uint16_t> measuredXYZ;
    PacketRespondLegAngles();
    void PackInner(ByteBufferWriter& writer) override;
};

struct PacketRequestIMUData : public Packet {
    PacketRequestIMUData();
};

struct PacketRespondIMUData : public Packet {
    PacketType type = PacketType::RespondIMUData;
    std::vector<float> acceleration;
    std::vector<float> up;
    std::vector<float> gyro;
    PacketRespondIMUData();
    void PackInner(ByteBufferWriter& writer) override;
};

}