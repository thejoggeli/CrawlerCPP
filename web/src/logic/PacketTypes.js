import Packet from "msl/remote/Packet.js"

export default function PacketTypes(){
    Packet.AddType({
        id: 0x0010, name: "CS_GamepadKey",
        pack: function(buffer, data){
            buffer.writeUint8(data.key)
            buffer.writeUint8(data.state)
        },
        unpack: function(buffer, data){
            data.key = buffer.readUint8()
            data.state = buffer.readUint8()
        },
    })
    Packet.AddType({
        id: 0x0011, name: "CS_GamepadJoystick",
        pack: function(buffer, data){
            buffer.writeUint8(data.key)
            buffer.writeFloat32(data.x)
            buffer.writeFloat32(data.y)
        },
        unpack: function(buffer, data){
            data.key = buffer.readUint8()
            data.x = buffer.readFloat32()
            data.y = buffer.readFloat32()
        },
    })
    Packet.AddType({
        id: 0x0020, name: "SC_ReadRobotPosLegAngles",
        unpack: function(buffer, data){
            data.numLegs = buffer.readUint8()
            data.legIds = []
            data.legAngles = []
            for(var i = 0; i < data.numLegs; i++){
                data.legIds.push(buffer.readUint8())
                data.legAngles.push([
                    buffer.readFloat32(), // H0   
                    buffer.readFloat32(), // K1
                    buffer.readFloat32(), // K2
                    buffer.readFloat32(), // K3
                ])
            }
        },
    })
}

