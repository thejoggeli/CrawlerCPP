#include "Packet.h"
#include "core/Log.h"
#include "util/ByteBufferReader.h"

namespace Crawler {

Packet::Packet(const uint8_t* bytes, unsigned int size){
    Unpack(bytes, size);
}

Packet::Packet(PacketType type){
    this->type = type;
    data.Add<uint16_t>("PacketType", (uint16_t)type);
}

void Packet::Unpack(const uint8_t* bytes, unsigned int size){


    // create byte buffer reader
    ByteBufferReader reader = ByteBufferReader(bytes, size);

    // read the packet type from the byte buffer (first 2 bytes) 
    type = (PacketType) reader.Read<uint16_t>();
    LogDebug("Packet", iLog << "unpacking " << *PacketTypeToString(type));

    // add packet type to smart buffer
    data.Add<uint16_t>("PacketType", (uint16_t)type);

    switch(type){
        case PacketType::GamepadKey: {
            data.Add<uint8_t>("key", reader.Read<uint8_t>());
            data.Add<uint8_t>("state", reader.Read<uint8_t>());
            break;
        }
        case PacketType::GamepadJoystick: {
            data.Add<uint8_t>("key", reader.Read<uint8_t>());
            data.Add<uint8_t>("state", reader.Read<uint8_t>());
            data.Add<float>("x", reader.Read<float>());
            data.Add<float>("y", reader.Read<float>());
            break;
        }
        default: {
            char buffer[10];
            sprintf(buffer, "%04X", (uint16_t)type);
            LogError("Packet", iLog << "invalid packet type " << buffer);
            break;
        }
    }

}



}