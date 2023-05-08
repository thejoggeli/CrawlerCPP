import React from "react";
import "./CalibComponent.css";
import CalibJoint from "./CalibJoint";
import Main from "logic/Main";
import PacketMessage from "logic/PacketMessage";
import Time from "msl/time/Time";

export default class CalibComponent extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            legs: [],
            joints: [],
        }
        this.anglePacketUnderway = false
        this.lastAngleUpdateTime = 0
        var legNames = ["Front-Left", "Back-Left", "Back-Right", "Front-Right"]
        var jointNames = ["H0", "K1", "K2", "K3"]
        for(var i = 0; i < 4; i++){
            var leg = {joints: [], index: i, name: legNames[i]}
            for(var j = 0; j < 4; j++){
                var joint = {
                    leg: leg,
                    id: i*4+j,
                    index: j,
                    name: jointNames[j],
                    measuredAngle: 0,
                    measuredXYZ: 0,
                    points: [
                        {value: 0, done: false, visible: true, calibAngle: 0},
                        {value: 0, done: false, visible: j!=0, calibAngle: 0},
                        {value: 0, done: false, visible: true, calibAngle: 0},
                    ]
                }
                leg.joints.push(joint)
                this.state.joints.push(joint)
            }
            this.state.legs.push(leg)
        }
    }

    componentDidMount(){
        var request = new PacketMessage("requestCalib")
        request.addInt("n", 16)
        for(var i = 0; i < 16; i++){
            request.addInt("j-"+i, i);
        }
        Main.addPacketMessage(request);
        Main.packetMessages.subscribe("respondCalib", this, this.handleRespondCalib)
        Main.packetMessages.subscribe("setCalibResponse", this, this.handleSetCalibResponse)
        Main.packetReceiver.subscribe("RespondLegAngles", this, this.handleRespondLegAngles)
        Main.events.subscribe("update", this, this.handleUpdate)
    }

    componentWillUnmount(){
        Main.packetMessages.unsubscribe("respondCalib", this)
        Main.packetMessages.unsubscribe("setCalibResponse", this)
        Main.packetReceiver.unsubscribe("RespondLegAngles", this)
        Main.events.unsubscribe("update", this)
    }

    handleUpdate(){
        // request new angles from server
        if(Time.currentTime - this.lastAngleUpdateTime > 0.05 && !this.anglePacketUnderway){
            this.lastAngleUpdateTime = Time.currentTime
            Main.addPacket("RequestLegAngles", {
                "legIds": [0, 1, 2, 3],
                "measuredAngle": true,
                "measuredXYZ": true,
            })
            this.anglePacketUnderway = true
        }
    }

    handleRespondCalib(msg){
        console.log(msg)
        // var legs = this.state.legs
        var numJoints = msg.getInt("n")
        for(var i = 0; i < numJoints; i++){
            var jointId = msg.getInt("j-"+i)
            var legIndex = Math.floor(jointId/4)
            var jointIndex = jointId%4
            this.state.legs[legIndex].joints[jointIndex].points[0].value = msg.getInt("l-"+jointId) // low
            this.state.legs[legIndex].joints[jointIndex].points[1].value = msg.getInt("m-"+jointId) // mid
            this.state.legs[legIndex].joints[jointIndex].points[2].value = msg.getInt("h-"+jointId) // high
            this.state.legs[legIndex].joints[jointIndex].points[0].calibAngle = msg.getFloat("cl-"+jointId) // low
            this.state.legs[legIndex].joints[jointIndex].points[1].calibAngle = msg.getFloat("cm-"+jointId) // mid
            this.state.legs[legIndex].joints[jointIndex].points[2].calibAngle = msg.getFloat("ch-"+jointId) // high
            // console.log(this.state.legs[legIndex].joints[jointIndex].points)
        }
        this.setState(this.state)
    }

    handleRespondLegAngles(packet){
        this.anglePacketUnderway = false
        var data = packet.data
        if(data.measuredAngle){
            for(var leg of data.legs){
                for(var j in leg.joints){
                    this.state.legs[leg.id].joints[j].measuredAngle = leg.joints[j].measuredAngle
                }
            }
        }   
        if(data.measuredXYZ){
            for(var leg of data.legs){
                for(var j in leg.joints){
                    this.state.legs[leg.id].joints[j].measuredXYZ = leg.joints[j].measuredXYZ
                }
            }
        }
        this.setState(this.state)
    }

    handleClick = e => {

    };

    onCalibrate = (joint, valueNumber) => {
        if(joint.calibMessageSent){
            console.log("calibrate already sent", joint)
            return
        }
        joint.points[valueNumber].calibMessageSent = true
        joint.points[valueNumber].done = false
        var msg = new PacketMessage("setCalib")
        msg.addInt("jointId", joint.id)
        msg.addInt("valueNumber", valueNumber)
        Main.addPacketMessage(msg)
        this.setState(this.state)
    }

    handleSetCalibResponse(msg){
        var jointId = msg.getInt("jointId")
        var valueNumber = msg.getInt("valueNumber")
        var joint = this.state.joints[jointId]
        joint.points[valueNumber].value = msg.getInt("value")
        joint.points[valueNumber].done = true
        joint.points[valueNumber].calibMessageSent = false
        this.setState(this.state)
    }

    render() {
        return (
            <div className="calib">
                {Array.from(this.state.legs).map((leg, legIdx) => {
                    var jointComponents = Array.from(leg.joints).map((joint, jointIdx) => {
                        return <CalibJoint joint={joint} key={joint.id} onCalibrate={this.onCalibrate} />
                    })
                    return (
                        <div key={leg.index} className="leg">
                            <div className="leg-name">{leg.name} ({leg.index})</div>
                            <table className="joints">
                                <thead>
                                    <tr>
                                        <th className="joint-name" key={"name"}>Joint</th>
                                        <th className="joint-name" key={0}>deg</th>
                                        <th className="joint-name" key={1}>uint16</th>
                                        <th className="joint-name" key={2}>Calib Low</th>
                                        <th className="joint-name" key={3}>Calib Mid</th>
                                        <th className="joint-name" key={4}>Calib High</th>
                                    </tr>
                                </thead>
                                <tbody>
                                    {jointComponents}
                                </tbody>
                            </table>
                        </div>
                    )
                })}
            </div>
        );
    }
}