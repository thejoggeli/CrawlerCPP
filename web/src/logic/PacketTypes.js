import Packet from "msl/remote/Packet.js"

export default function PacketTypes(){
    Packet.AddType({
        id: 0x0010, name: "GamepadKey",
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
        id: 0x1100, name: "GamepadJoystick",
        pack: function(buffer, data){
            buffer.writeUint8(data.key)
            buffer.writeUint8(data.state)
            buffer.writeFloat32(data.x)
            buffer.writeFloat32(data.y)
        },
        unpack: function(buffer, data){
            data.key = buffer.readUint8()
            data.state = buffer.readUint8()
            data.x = buffer.readFloat32()
            data.y = buffer.readFloat32()
        },
    })
}

