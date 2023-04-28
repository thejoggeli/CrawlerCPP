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
        id: 0x0030, name: "CS_RequestLegData",
        pack: function(buffer, data){
            buffer.writeUint8(data.dataType)
            buffer.writeUint8(data.legIds.length)
            for(var legId of data.legIds){
                buffer.writeUint8(legId)
            }
        },
    })
    Packet.AddType({
        id: 0x0031, name: "SC_RespondLegData",
        unpack: function(buffer, data){
            data.dataType = buffer.readUint8()
            data.numLegs = buffer.readUint8()
            data.legs = []
            // loop through legs
            for(var i = 0; i < data.numLegs; i++){
                // create leg container
                var leg = {
                    id: buffer.readUint8(),
                    joints: [],
                    distance: 0,
                    weight: 0,
                }
                data.legs.push(leg)
                // read joint data
                for(var j = 0; j < 4; j++){
                    switch(data.dataType){
                        case 0:
                            leg.joints.push({
                                "angle": buffer.readFloat32(),
                            })
                            break
                        case 1:
                            leg.joints.push({
                                "targetAngle": buffer.readFloat32(),
                                "measuredAngle": buffer.readFloat32(),
                                "current": buffer.readFloat32(),
                                "pwm": buffer.readFloat32(),
                                "temperature": buffer.readFloat32(),
                                "voltage": buffer.readFloat32(),
                                "statusDetail": buffer.readUint8(),
                                "statusError": buffer.readUint8(),
                            })
                            break
                    }
                }
                // read leg data
                switch(data.dataType){
                    case 1:
                        leg.distance = buffer.readFloat32()
                        leg.weight = buffer.readFloat32()
                        break;
                }
            }
        },
    })
    Packet.AddType({
        id: 0x0040, name: "CS_RequestIMUData",
        pack: function(buffer, data){
            // nothing
        },
    })
    Packet.AddType({
        id: 0x0041, name: "SC_RespondIMUData",
        unpack: function(buffer, data){
            data.acceleration = {
                x: buffer.readFloat32(),
                y: buffer.readFloat32(),
                z: buffer.readFloat32(),
            }
            data.gyro = {
                x: buffer.readFloat32(),
                y: buffer.readFloat32(),
                z: buffer.readFloat32(),
            }
        },
    })
}

