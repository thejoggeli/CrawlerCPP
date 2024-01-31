import Packet from "msl/remote/Packet.js"
import Bitwise from "msl/util/Bitwise"

export default function PacketTypes(){
    Packet.AddType({
        id: 0x0002, name: "RequestStatus",
        pack: function(buffer, data){},
    })
    Packet.AddType({
        id: 0x0003, name: "RespondStatus",
        unpack: function(buffer, data){
            data.fps = buffer.readFloat32()
            data.fixedFps = buffer.readFloat32()
            data.time = buffer.readFloat32()
            data.clients = buffer.readUint16()
        },
    })
    Packet.AddType({
        id: 0x0008, name: "Message",
        pack: function(buffer, data){
            buffer.writeUint16(Object.keys(data.params).length)
            buffer.writeString(data.message)
            for(var key in data.params){
                buffer.writeString(key)
                buffer.writeString(data.params[key])
            }
        },
        unpack: function(buffer, data){
            var numParams = buffer.readUint16()
            data.message = buffer.readString()
            data.params = {}
            for(var i = 0; i < numParams; i++){
                var key = buffer.readString()
                var value = buffer.readString()
                data.params[key] = value
            }
        },
    })
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
        id: 0x0011, name: "GamepadJoystick",
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
        id: 0x0020, name: "RequestSetJointPosition",
        pack: function(buffer, data){
            buffer.writeUint8(data.leg)
            buffer.writeUint8(data.joint)
            buffer.writeFloat32(data.x)
            buffer.writeFloat32(data.y)
            buffer.writeFloat32(data.z)
        },
    })
    Packet.AddType({
        id: 0x0021, name: "RequestSetLegAngles",
        pack: function(buffer, data){
            buffer.writeUint8(data.leg)
            for(var i = 0; i < data.angles; i++){
                buffer.writeFloat32(data.angles[i])
            }
        },
    })
    Packet.AddType({
        id: 0x0030, name: "RequestLegData",
        pack: function(buffer, data){
            buffer.writeUint8(data.legIds.length)
            for(var legId of data.legIds){
                buffer.writeUint8(legId)
            }
        },
    })
    Packet.AddType({
        id: 0x0031, name: "RespondLegData",
        unpack: function(buffer, data){
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
                }
                // read leg data
                leg.distance = buffer.readFloat32()
                leg.weight = buffer.readFloat32()
            }
        },
    })
    Packet.AddType({
        id: 0x0032, name: "RequestLegAngles",
        pack: function(buffer, data){
            buffer.writeUint8(data.legIds.length)
            var flags = 0
            flags = Bitwise.setBitTo(flags, 0, data.targetAngle)
            flags = Bitwise.setBitTo(flags, 1, data.measuredAngle)
            flags = Bitwise.setBitTo(flags, 2, data.targetXYZ)
            flags = Bitwise.setBitTo(flags, 3, data.measuredXYZ)
            buffer.writeUint8(flags)
            for(var legId of data.legIds){
                buffer.writeUint8(legId)
            }
        },
    })
    Packet.AddType({
        id: 0x0033, name: "RespondLegAngles",
        unpack: function(buffer, data){
            data.numLegs = buffer.readUint8()
            data.legs = []
            var flags = buffer.readUint8()
            data.targetAngle = Bitwise.isBitSet(flags, 0)
            data.measuredAngle = Bitwise.isBitSet(flags, 1)
            data.targetXYZ = Bitwise.isBitSet(flags, 2)
            data.measuredXYZ = Bitwise.isBitSet(flags, 3)
            data.flags = flags
            // loop through legs
            for(var i = 0; i < data.numLegs; i++){
                // create leg container
                var leg = {
                    id: buffer.readUint8(),
                    joints: [],
                }
                data.legs.push(leg)
                for(var j = 0; j < 4; j++){
                    leg.joints.push({
                        "targetAngle": null,
                        "measuredAngle": null,
                        "targetXYZ": null,
                        "measuredXYZ": null,
                    })
                }
            }
            if(data.targetAngle){
                for(var i = 0; i < data.numLegs; i++){
                    for(var j = 0; j < 4; j++){
                        data.legs[i].joints[j].targetAngle = buffer.readFloat32()
                    }
                }
            }
            if(data.measuredAngle){
                for(var i = 0; i < data.numLegs; i++){
                    for(var j = 0; j < 4; j++){
                        data.legs[i].joints[j].measuredAngle = buffer.readFloat32()
                    }
                }
            }
            if(data.targetXYZ){
                for(var i = 0; i < data.numLegs; i++){
                    for(var j = 0; j < 4; j++){
                        data.legs[i].joints[j].targetXYZ = buffer.readUint16()
                    }
                }
            }
            if(data.measuredXYZ){
                for(var i = 0; i < data.numLegs; i++){
                    for(var j = 0; j < 4; j++){
                        data.legs[i].joints[j].measuredXYZ = buffer.readUint16()
                    }
                }
            }
        },
    })
    Packet.AddType({
        id: 0x0040, name: "RequestIMUData",
        pack: function(buffer, data){
            // nothing
        },
    })
    Packet.AddType({
        id: 0x0041, name: "RespondIMUData",
        unpack: function(buffer, data){
            data.acceleration = {
                x: buffer.readFloat32(),
                y: buffer.readFloat32(),
                z: buffer.readFloat32(),
            }
            data.up = {
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

