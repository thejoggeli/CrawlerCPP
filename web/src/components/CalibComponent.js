import React from "react";
import "./CalibComponent.css";
import CalibJoint from "./CalibJoint";
import Main from "logic/Main";
import PacketMessage from "logic/PacketMessage";

export default class CalibComponent extends React.Component {

    constructor(props){
        super(props);
        this.state = {

        }
        this.legs = []
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
                }
                leg.joints.push(joint)
            }
            this.legs.push(leg)
        }
    }

    componentDidMount(){
        var request = new PacketMessage("requestCalib")
        request.addInt("n", 16)
        for(var i = 0; i < 16; i++){
            request.addInt("j-"+i, i);
        }
        Main.addPacketMessage(request);
        Main.packetMessages.subscribe("respondCalib", this, this.respondCalibHandler)
    }

    componentWillUnmount(){
        Main.packetMessages.unsubscribe("respondCalib", this)
    }

    respondCalibHandler(msg){
        console.log(msg)
        // var legs = this.state.legs
        var numJoints = msg.getInt("n")
        for(var i = 0; i < numJoints; i++){
            var jointId = msg.getInt("j-"+i)
            var v_low = msg.getInt("l-"+jointId) // low
            var v_mid = msg.getInt("m-"+jointId) // mid
            var v_high = msg.getInt("h-"+jointId) // high
            var legIndex = Math.floor(jointId/4)
            var jointIndex = jointId%4
            console.log(jointId, legIndex, jointIndex, v_low, v_mid, v_high)
            // legs[legIndex].joints[jointIndex].
        }
    }

    handleClick = e => {

    };

    render() {
        console.log("render")
        return (
            <div className="calib">
                {Array.from(this.legs).map((leg, legIdx) => {
                    var jointComponents = Array.from(leg.joints).map((joint, jointIdx) => {
                        return <CalibJoint joint={joint} key={joint.id} />
                    })
                    return (
                        <div key={leg.index} className="leg">
                            <div className="leg-name">{leg.name} ({leg.index})</div>
                            <table className="joints">
                                <thead>
                                    <tr>
                                        <th className="joint-name" key={"name"}>Joint</th>
                                        <th className="joint-name" key={0}>-90°</th>
                                        <th className="joint-name" key={1}>0°</th>
                                        <th className="joint-name" key={2}>°90°</th>
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