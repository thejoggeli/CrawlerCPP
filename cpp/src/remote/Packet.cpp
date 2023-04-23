#include "Packet.h"
#include "core/Log.h"
#include "util/ByteBufferReader.h"
#include "util/Endian.h"

namespace Crawler {

Packet::Packet(PacketType type){
    this->type = type;
    if(IsBigEndian()){
        data.swapEndian = true;
        LogDebug("Packet", "SwapEndian");
    }
    data.Add<uint16_t>("PacketType", (uint16_t)type);
}

std::shared_ptr<Packet> Packet::Unpack(const uint8_t* bytes, unsigned int size){

    // create byte buffer reader
    ByteBufferReader reader = ByteBufferReader(bytes, size);

    if(IsBigEndian()){
        reader.swapEndian = true;
        LogDebug("Packet", "SwapEndian");
    }

    // read the packet type from the byte buffer (first 2 bytes) 
    PacketType type = (PacketType) reader.Read<uint16_t>();
    LogDebug("Packet", iLog << "unpacking " << *PacketTypeToString(type));

    // create packet
    std::shared_ptr<Packet> packet = std::make_shared<Packet>(type);
    switch(type){
        case PacketType::CS_GamepadKey: {
            packet->data.Add<uint8_t>("key", reader.Read<uint8_t>());
            packet->data.Add<uint8_t>("state", reader.Read<uint8_t>());
            break;
        }
        case PacketType::CS_GamepadJoystick: {
            packet->data.Add<uint8_t>("key", reader.Read<uint8_t>());
            packet->data.Add<float>("x", reader.Read<float>());
            packet->data.Add<float>("y", reader.Read<float>());
            break;
        }
        default: {
            char buffer[10];
            sprintf(buffer, "%04X", (uint16_t)type);
            LogError("Packet", iLog << "invalid packet type " << buffer);
            return nullptr;
        }
    }

    return packet;

}



}