#include "Packet.h"
#include "core/Log.h"
#include "buffer/ByteBufferReader.h"
#include "util/Endian.h"

using namespace std;

namespace Crawler {

uint32_t Packet::nextPacketId = 0;

Packet::Packet(PacketType type) {
    this->type = type;
}

std::shared_ptr<Packet> Packet::Unpack(const uint8_t* bytes, unsigned int size, int clientId){

    // create byte buffer reader
    ByteBufferReader reader = ByteBufferReader(bytes, size);

    // read the packet type from the byte buffer (first 2 bytes) 
    PacketType type = (PacketType) reader.Read<uint16_t>();
    // LogDebug("Packet", iLog << "unpacking " << *PacketTypeToString(type) << " client=" << clientId);

    // read the packet id
    uint32_t id = reader.Read<uint32_t>();

    // abort if PacketType read failed
    if(reader.errors > 0){
        LogError("Packet", "unpacking reading PacketType failed");
        return nullptr;
    }

    std::shared_ptr<Packet> packet = CreatePacketFromType(type);

    if(!packet){
        LogError("Packet", iLog << *PacketTypeToString(type) << " unpacking failed: packet was nullptr");
        return nullptr;
    }

    if(packet->type == PacketType::Invalid){
        LogError("Packet", iLog << *PacketTypeToString(type) << " unpacking failed: invalid packet type");
        return nullptr;
    }

    packet->id = id;
    packet->clientId = clientId;
    packet->UnpackInner(reader);

    if(reader.errors > 0){
        LogError("Packet", iLog << *PacketTypeToString(type) << " unpacking failed: reader has errors");
        return nullptr;
    }

    return packet;

}

void Packet::PackInner(ByteBufferWriter& writer){

}

void Packet::UnpackInner(ByteBufferReader& reader){

}

void Packet::Pack(std::vector<uint8_t>& buffer){
    ByteBufferWriter writer(&buffer);
    writer.Write<uint16_t>((uint16_t)type);
    writer.Write<uint32_t>(nextPacketId);
    PackInner(writer);
    nextPacketId += 1;
}

// PacketTypedMessage
PacketMessage::PacketMessage() : Packet(PacketType::Message){
    this->message = message;
}
PacketMessage::PacketMessage(const char* message) : Packet(PacketType::Message){
    this->message = message;
}
void PacketMessage::PackInner(ByteBufferWriter& writer) {
    writer.Write<uint16_t>(params.size()); // numParams
    writer.WriteString(message.c_str());
    for (auto const& x: params){
        writer.WriteString(x.first.c_str());
        writer.WriteString(x.second.c_str());
    }
}
void PacketMessage::UnpackInner(ByteBufferReader& reader) {
    unsigned int numParams = reader.Read<uint16_t>();
    message = reader.ReadString();
    for(int i = 0; i < numParams; i++){
        const char* key = reader.ReadString();   
        const char* val = reader.ReadString();
        params[key] = val;
    } 
}

void PacketMessage::SetMessage(const char* message){
    this->message = message;
}

void PacketMessage::AddFloat(const std::string& key, float value){
    char buffer[20];
    sprintf(buffer, "%.8g", value);
    params[key] = buffer;
}

void PacketMessage::AddInt(const std::string& key, int value){
    params[key] = std::to_string(value);
}

void PacketMessage::AddBool(const std::string& key, bool value){
    params[key] = value ? '1' : '0';
}

void PacketMessage::AddString(const std::string& key, const char* string){
    params[key] = string;
}

float PacketMessage::GetFloat(const std::string& key){
    if(!params.contains(key)){
        LogError("Packet", iLog << "invalid param key in PacketMessage: key=" << key);
        return 0;
    }
    return std::stof(params[key]);
}

int PacketMessage::GetInt(const std::string& key){
    if(!params.contains(key)){
        LogError("Packet", iLog << "invalid param key in PacketMessage: key=" << key);
        return 0;
    }
    return std::stoi(params[key]);
}

bool PacketMessage::GetBool(const std::string& key){
    if(!params.contains(key)){
        LogError("Packet", iLog << "invalid param key in PacketMessage: key=" << key);
        return false;
    }
    return params[key] == "0" ? false : true; 
}

const char* PacketMessage::GetString(const std::string& key){
    if(!params.contains(key)){
        LogError("Packet", iLog << "invalid param key in PacketMessage: key=" << key);
        return "";
    }
    return params[key].c_str();
}

// PacketGamepadKey
PacketGamepadKey::PacketGamepadKey() : Packet(PacketType::GamepadKey){}
void PacketGamepadKey::PackInner(ByteBufferWriter& writer){
    writer.Write<uint8_t>((uint8_t)key);
    writer.Write<uint8_t>((uint8_t)state);
}
void PacketGamepadKey::UnpackInner(ByteBufferReader& reader){ 
    key = (GamepadKey) reader.Read<uint8_t>();
    state = (GamepadKeyState) reader.Read<uint8_t>();
}

// PacketGamepadJoystick
PacketGamepadJoystick::PacketGamepadJoystick() : Packet(PacketType::GamepadJoystick){}
void PacketGamepadJoystick::PackInner(ByteBufferWriter& writer){
    writer.Write<uint8_t>((uint8_t)key);
    writer.Write<float>(x);
    writer.Write<float>(y);
}
void PacketGamepadJoystick::UnpackInner(ByteBufferReader& reader){ 
    key = (GamepadKey) reader.Read<uint8_t>();
    x = reader.Read<float>();
    y = reader.Read<float>();
}

// PacketRequestSetJointPosition
PacketRequestSetJointPosition::PacketRequestSetJointPosition() : Packet(PacketType::RequestSetJointPosition){}
void PacketRequestSetJointPosition::UnpackInner(ByteBufferReader& reader){
    leg = reader.Read<uint8_t>();
    joint = reader.Read<uint8_t>();
    x = reader.Read<float>();
    y = reader.Read<float>();
    z = reader.Read<float>();
}

// PacketRequestSetLegAngles
PacketRequestSetLegAngles::PacketRequestSetLegAngles() : Packet(PacketType::RequestSetLegAngles){}
void PacketRequestSetLegAngles::UnpackInner(ByteBufferReader& reader){
    leg = reader.Read<uint8_t>();
    for(int i = 0; i < 4; i++){
        angles.push_back(reader.Read<float>());
    }
}

// PacketRequestLegData
PacketRequestLegData::PacketRequestLegData() : Packet(PacketType::RequestLegData){}
void PacketRequestLegData::UnpackInner(ByteBufferReader& reader){
    unsigned int numLegs = reader.Read<uint8_t>();
    for(int i = 0; i < numLegs; i++){
        legIds.push_back(reader.Read<uint8_t>());
    }
}

// PacketRespondLegData
PacketRespondLegData::PacketRespondLegData() : Packet(PacketType::RespondLegData){}
void PacketRespondLegData::PackInner(ByteBufferWriter& writer){
    writer.Write<uint8_t>(legIds.size());
    int k = 0;
    for(int i = 0; i < legIds.size(); i++){
        writer.Write<uint8_t>(legIds[i]);
        for(int j = 0; j < 4; j++){
            writer.Write<float>(targetAngle[k]);
            writer.Write<float>(measuredAngle[k]);
            writer.Write<float>(current[k]);
            writer.Write<float>(pwm[k]);
            writer.Write<float>(temperature[k]);
            writer.Write<float>(voltage[k]);
            writer.Write<uint8_t>(statusDetail[k]);
            writer.Write<uint8_t>(statusError[k]);
            k++;
        }
        writer.Write<float>(distance[i]);
        writer.Write<float>(weight[i]);
    }
}

// PacketRequestLegAngles
PacketRequestLegAngles::PacketRequestLegAngles() : Packet(PacketType::RequestLegAngles){}
void PacketRequestLegAngles::UnpackInner(ByteBufferReader& reader){
    unsigned int numLegs = reader.Read<uint8_t>();
    flags = reader.Read<uint8_t>();
    for(int i = 0; i < numLegs; i++){
        legIds.push_back(reader.Read<uint8_t>());
    }
}

// PacketRespondLegAngles
PacketRespondLegAngles::PacketRespondLegAngles() : Packet(PacketType::RespondLegAngles){}
void PacketRespondLegAngles::PackInner(ByteBufferWriter& writer){
    writer.Write<uint8_t>(legIds.size());
    writer.Write<uint8_t>(flags);
    int k = 0;
    writer.WriteVector<uint8_t>(legIds);
    if((flags>>0)&1){
        writer.WriteVector<float>(targetAngle);
    }
    if((flags>>1)&1){
        writer.WriteVector<float>(measuredAngle);
    }
    if((flags>>2)&1){
        writer.WriteVector<uint16_t>(targetXYZ);
    }
    if((flags>>3)&1){
        writer.WriteVector<uint16_t>(measuredXYZ);
    }
}

// PacketRequestIMUData
PacketRequestIMUData::PacketRequestIMUData() : Packet(PacketType::RequestIMUData){}

// PacketRespondIMUData
PacketRespondIMUData::PacketRespondIMUData() : Packet(PacketType::RespondIMUData){}
void PacketRespondIMUData::PackInner(ByteBufferWriter& writer){
    writer.WriteVector<float>(acceleration);
    writer.WriteVector<float>(gyro);
}

}