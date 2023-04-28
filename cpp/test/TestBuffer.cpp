#include "core/Log.h"
#include "util/SmartBuffer.h"
#include "remote/Packet.h"
#include "remote/GamepadKeys.h"

using namespace Crawler;

void testPacket(){

    SmartBuffer buffer = SmartBuffer();

    buffer.Add<uint16_t>("type", (uint16_t)PacketType::CS_GamepadJoystick);
    buffer.Add<uint8_t>("key", (uint8_t)GamepadKey::LeftJoystick);
    buffer.Add<float>("x", 0.1337f);
    buffer.Add<float>("y", 4.2f);

    std::shared_ptr<Packet> packet = Packet::Unpack(buffer.GetBytes(), buffer.GetSize());

    LogInfo("TestPacket", iLog << *PacketTypeToString(packet->type));
    LogInfo("TestPacket", iLog << *GamepadKeyToString(packet->data.Get<uint8_t>("key")));
    LogInfo("TestPacket", iLog << packet->data.Get<float>("x"));
    LogInfo("TestPacket", iLog << packet->data.Get<float>("y"));
    
    std::shared_ptr<Packet> packet2 = Packet::Create(PacketType::CS_GamepadKey);
    packet2->data.Add<uint8_t>(13);
    packet2->data.Add<uint8_t>(37);

    std::shared_ptr<Packet> packet3 = Packet::Unpack(packet2->data.GetBytes(), packet2->data.GetSize());

    LogInfo("TestPacket", iLog << *PacketTypeToString(packet3->type));
    LogInfo("TestPacket", iLog << *GamepadKeyToString(packet3->data.Get<uint8_t>("key")));

}

void testSmartBuffer(){

    SmartBuffer buffer = SmartBuffer();

    buffer.Add<float>("v1", 1337.3383f);
    buffer.AddString("str1", "hallo welt");
    buffer.Add<float>("v2", -1.5383f);
    buffer.AddString("str2", "hallo welt xyz");
    buffer.Add<float>("v3", 0.0f);
    buffer.Add<uint16_t>("v4", 512);
    buffer.Add<float>("x", 1);
    buffer.Add<float>("x", 2);
    buffer.Add<float>("x", 3);

    float v1 = buffer.Get<float>("v1");
    std::string str1 = buffer.GetString("str1");
    float v2 = buffer.Get<float>("v2");
    float v3 = buffer.Get<float>("v3");
    std::string str2 = buffer.GetString("str2");
    uint16_t v4 = buffer.Get<uint16_t>("v4");

    float x1 = buffer.Get<float>("x", 0);
    float x2 = buffer.Get<float>("x", 1);
    float x3 = buffer.Get<float>("x", 2);

    LogInfo("TestSmartBuffer", iLog << v1);
    LogInfo("TestSmartBuffer", iLog << v2);
    LogInfo("TestSmartBuffer", iLog << v3);
    LogInfo("TestSmartBuffer", iLog << v4);
    LogInfo("TestSmartBuffer", iLog << str1);
    LogInfo("TestSmartBuffer", iLog << str2);
    LogInfo("TestSmartBuffer", iLog << x1);
    LogInfo("TestSmartBuffer", iLog << x2);
    LogInfo("TestSmartBuffer", iLog << x3);

}


int main(){

    testSmartBuffer();

    testPacket();

    return EXIT_SUCCESS;
}