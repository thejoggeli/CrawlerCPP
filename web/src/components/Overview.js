import React from "react";
import "./Overview.css";
import Robot3D from "./Robot3D"
import Main from "logic/Main"
import GamepadCanvas from "./GamepadCanvas";
import { vec3 } from "gl-matrix";
import Numbers from "msl/util/Numbers";
import Strings from "msl/util/Strings";
import Time from "msl/time/Time";

export default class Overview extends React.Component {

    constructor(props){

        super(props);

        this.lastUpdate = Time.currentTime
        
        // initialize state
        this.state = {
            "webTime": 0,
            "robotTime": 0,
            "legs": [],
            "imu": {
                "acceleration": vec3.create(0, 0, 0),
                "gyro": vec3.create(0, 0, 0),
            },
            "currentSumAll": 0,
            "currentSumJoints": [0, 0, 0, 0],
            "temperatureMeanAll": 0,
            "temperatureMeanJoints": [0, 0, 0, 0],
        }

        // add legs to state
        this.legNames = ["Front-Left", "Back-Left", "Back-Right", "Front-Right"]
        this.legNamesShort = ["FL", "BL", "BR", "FR"]
        this.jointNamesShort = ["H0", "K1", "K2", "K3"]
        for(var i = 0; i < 4; i++){
            var leg = {
                "id": i,
                "name": this.legNames[i],
                "shortName": this.legNamesShort[i],
                "joints": [],
                "angleErrorSum": 0,
                "currentSum": 0,
                "temperatureMean": 0,
                "weight": 0,
                "distance": 0,
            }
            for(var j = 0; j < 4; j++){
                var joint = {
                    "id": j,
                    "shortName": this.jointNamesShort[j],
                    "targetAngle": 0,
                    "measuredAngle": 0,
                    "angleError": 0,
                    "current": 0,
                    "voltage": 0,
                    "temperature": 0,
                    "statusError": 0,
                    "statusDetail": 0,
                    "pwm": 0,
                }
                leg.joints.push(joint)
            }
            this.state.legs.push(leg)
        }
        this.computeJointAngleErrors(this.state.legs)
        this.computeCurrents(this.state)
        this.computeTemperatures(this.state)

        this.imuDataPacketSent = false
        this.legDataPacketSent = false

    }

    computeJointAngleErrors(legs){
        for(var leg of legs){
            leg.angleErrorSum = 0.0
            for(var joint of leg.joints){
                joint.angleError = joint.measuredAngle - joint.targetAngle
                leg.angleErrorSum += Math.abs(joint.angleError)
            }
        }
    }

    computeCurrents(state){
        // current sum all
        state.currentSumAll = 0
        for(var leg of state.legs){
            for(var joint of leg.joints){
                state.currentSumAll += joint.current
            }
        }
        // current sum legs
        for(var leg of state.legs){
            leg.currentSum = 0
            for(var joint of leg.joints){
                leg.currentSum += joint.current
            }
        }
        // current sum joints
        for(var i = 0; i < 4; i++){
            state.currentSumJoints[i] = 0
            for(var j = 0; j < 4; j++){
                state.currentSumJoints[i] += state.legs[j].joints[i].current
            }
        }
    }

    computeTemperatures(state){
        // temperature mean all
        state.temperatureMeanAll = 0
        for(var leg of state.legs){
            for(var joint of leg.joints){
                state.temperatureMeanAll += joint.temperature
            }
        }
        state.temperatureMeanAll /= 16.0
        // temperature mean legs
        for(var leg of state.legs){
            leg.temperatureMean = 0
            for(var joint of leg.joints){
                leg.temperatureMean += joint.temperature
            }
            leg.temperatureMean /= 4.0
        }
        // temperature mean joints
        for(var i = 0; i < 4; i++){
            state.temperatureMeanJoints[i] = 0
            for(var j = 0; j < 4; j++){
                state.temperatureMeanJoints[i] += state.legs[j].joints[i].temperature
            }
            state.temperatureMeanJoints[i] /= 4.0
        }
    }

    componentDidMount(){
        Main.events.subscribe("update", this, this.handleUpdate)
        Main.packetReceiver.subscribe("SC_RespondLegData", this, this.handleRespondLegData)
        Main.packetReceiver.subscribe("SC_RespondIMUData", this, this.handleRespondIMUData)
    }

    componentWillUnmount(){
        Main.events.unsubscribe("update", this)
        Main.packetReceiver.unsubscribe("SC_RespondLegData", this)
        Main.packetReceiver.unsubscribe("SC_RespondIMUData", this)
    }

    handleRespondLegData(packet){
        this.legDataPacketSent = false
        var data = packet.data
        var state = this.state
        for(var leg of data.legs){
            for(var j in leg.joints){
                var joint = leg.joints[j]
                state.legs[leg.id].joints[j].targetAngle = joint.targetAngle
                state.legs[leg.id].joints[j].measuredAngle = joint.measuredAngle
                state.legs[leg.id].joints[j].temperature = joint.temperature
                state.legs[leg.id].joints[j].current = joint.current
                state.legs[leg.id].joints[j].voltage = joint.voltage
                state.legs[leg.id].joints[j].statusError = joint.statusError
                state.legs[leg.id].joints[j].statusDetail = joint.statusDetail
                state.legs[leg.id].joints[j].pwm = joint.pwm
            }
            state.legs[leg.id].weight = leg.weight
            state.legs[leg.id].distance = leg.distance
        }
        this.computeJointAngleErrors(state.legs)
        this.computeCurrents(state)
        this.computeTemperatures(state)
        this.setState(state)
    }

    handleRespondIMUData(packet){
        this.imuDataPacketSent = false
        var data = packet.data
        var state = this.state
        state.imu.acceleration.x = data.acceleration.x
        state.imu.acceleration.y = data.acceleration.y
        state.imu.acceleration.z = data.acceleration.z
        state.imu.gyro.x = data.gyro.x
        state.imu.gyro.y = data.gyro.y
        state.imu.gyro.z = data.gyro.z
        this.setState(state)
    }

    handleUpdate = () => {
        this.setState({
            webTime: Numbers.roundToFixed(Time.currentTime, 1),
        })
        if(Time.currentTime - this.lastUpdate > 0.1){
            // var state = this.state
            // for(var leg of state.legs){
            //     for(var joint of leg.joints){
            //         joint.targetAngle = Numbers.randomFloat(-Math.PI*0.5, Math.PI*0.5)
            //         joint.measuredAngle = Numbers.randomFloat(-Math.PI*0.5, Math.PI*0.5)
            //         joint.temperature = Numbers.randomFloat(1.0, 99.0)
            //         joint.current = Numbers.randomFloat(1.0, 99.0)
            //         joint.voltage = Numbers.randomFloat(1.0, 99.0)
            //         joint.statusError = Numbers.randomInt(0, 256)
            //         joint.statusDetail = Numbers.randomInt(0, 256)
            //     }
            //     leg.weight = Numbers.randomFloat(0.0, 5.0)
            //     leg.distance = Numbers.randomFloat(10.0, 100.0)
            // }
            // this.computeJointAngleErrors(state.legs)
            // this.computeCurrents(state)
            // this.computeTemperatures(state)
            // this.setState(state)
            // this.lastUpdate = Time.currentTime
            
            if(!this.legDataPacketSent){
                Main.addPacket("CS_RequestLegData", {
                    "dataType": 1,
                    "legIds": [0, 1, 2, 3],
                })
                this.legDataPacketSent = true
            }
            
            if(!this.imuDataPacketSent){
                Main.addPacket("CS_RequestIMUData", {})
                this.imuDataPacketSent = true
            }

            this.lastUpdate = Time.currentTime

        }
    }

    render() {
        return (
            <div className="overview">    
                <div>WebTime: {this.state.webTime}</div>
                <div>RobotTime: {this.state.robotTime}</div>
                <table>
                    <thead>
                        <tr key={"angles.h1"}>
                            <th key={"angles.h1.1"} colSpan={2}>Leg</th>
                            <th key={"angles.h1.2"} colSpan={4}>TargetAngle (deg)</th>
                            <th key={"angles.h1.3"} colSpan={4}>MeasuredAngle (deg)</th>
                            <th key={"angles.h1.4"} colSpan={5}>Error (deg)</th>
                        </tr>
                        <tr key={"angles.h2"}>
                            <th key={"angles.h2.id"}>ID</th>
                            <th key={"angles.h2.name"}>Name</th>
                            {this.jointNamesShort.map((x, i) => {
                                return <th key={"angles.h2.targetAngle."+i}>{x}</th>
                            })}
                            {this.jointNamesShort.map((x, i) => {
                                return <th key={"angles.h2.measuredAngle."+i}>{x}</th>
                            })}
                            {this.jointNamesShort.map((x, i) => {
                                return <th key={"angles.h2.angleError."+i}>{x}</th>
                            })}
                            <th key={"angles.h2.sum"}>SumAbs</th>
                        </tr>
                    </thead>
                    <tbody>
                        {this.state.legs.map((leg, i) => {
                            return (
                                <tr key={"angles.leg" + i}>
                                    <td className="leg" key={"angles.leg.id"+i}>{leg.id}</td>
                                    <td className="leg" key={"angles.leg.name"+i}>{leg.name}</td>
                                    {leg.joints.map((joint, j) => {
                                        return <td key={"angles.leg"+i + ".joint"+j + ".targetAngle"}>{
                                            Numbers.roundToFixed(joint.targetAngle*Numbers.rad2deg, 1)
                                        }</td>
                                    })}
                                    {leg.joints.map((joint, j) => {
                                        return <td key={"angles.leg"+i + ".joint"+j + ".measuredAngle"}>{
                                            Numbers.roundToFixed(joint.measuredAngle*Numbers.rad2deg, 1)
                                        }</td>
                                    })}
                                    {leg.joints.map((joint, j) => {
                                        return <td key={"angles.leg"+i + ".joint"+j + ".angleError"}>{
                                            Numbers.roundToFixed(joint.angleError*Numbers.rad2deg, 1)
                                        }</td>
                                    })}
                                    <td key={"angles.leg"+i + ".sum"}>{
                                        Numbers.roundToFixed(leg.angleErrorSum*Numbers.rad2deg, 1)
                                    }</td>
                                </tr>
                            )
                        })}
                    </tbody>
                </table>
                <table>
                    <thead>
                        <tr key={"servos.h1"}>
                            <th key={"servos.h1.1"} colSpan={2}>Leg</th>
                            <th key={"servos.h1.2"} colSpan={5}>Current (mA)</th>
                            <th key={"servos.h1.3"} colSpan={4}>Voltage (V)</th>
                            <th key={"servos.h1.4"} colSpan={5}>Temperature (degC)</th>
                        </tr>
                        <tr key={"servos.h2"}>
                            <th key={"servos.h2.id"}>ID</th>
                            <th key={"servos.h2.name"}>Name</th>
                            {this.jointNamesShort.map((x, i) => {
                                return <th key={"servos.h2.current"+i}>{x}</th>
                            })}
                            <th key="servos.h2.sum">Sum</th>
                            {this.jointNamesShort.map((x, i) => {
                                return <th key={"servos.h2.voltage"+i}>{x}</th>
                            })}
                            {this.jointNamesShort.map((x, i) => {
                                return <th key={"servos.h2.temperature"+i}>{x}</th>
                            })}
                            <th key={"servos.h2.mean"}>Mean</th>
                        </tr>
                    </thead>
                    <tbody>
                        {this.state.legs.map((leg, i) => {
                            return (
                                <tr key={"servos.leg"+i}>
                                    <td className="leg" key={"servos.leg.id"+i}>{leg.id}</td>
                                    <td className="leg" key={"servos.leg.name"+i}>{leg.name}</td>
                                    {leg.joints.map((joint, j) => {
                                        return <td key={"servos.leg"+i + ".joint"+j + ".current"} className="current">{
                                            Numbers.roundToFixed(joint.current, 1)
                                        }</td>
                                    })}
                                    <td key={"servos.leg"+i+".current.sum"} className="sum">{
                                        Numbers.roundToFixed(leg.currentSum, 1)
                                    }</td>
                                    {leg.joints.map((joint, j) => {
                                        return <td key={"servos.leg"+i + ".joint"+j + ".voltage"} className="voltage">{
                                            Numbers.roundToFixed(joint.voltage, 1)
                                        }</td>
                                    })}
                                    {leg.joints.map((joint, j) => {
                                        return <td key={"servos.leg"+i + ".joint"+j + ".temperature"} className="temperature">{
                                            Numbers.roundToFixed(joint.temperature, 1) 
                                        }</td>
                                    })}
                                    <td key={"servos.leg"+i+".temperature.mean"} className="sum">{
                                        Numbers.roundToFixed(leg.temperatureMean, 1)
                                    }</td>
                                </tr>
                            )
                        })}
                        <tr className="tr-sum" key={"servos.foot"}>
                            <td key={"servos.foot.sum"} colSpan={2}>Sum</td>
                            {this.state.currentSumJoints.map((val, i) => {
                                return <td key={"servos.foot.current"+i}>{Numbers.roundToFixed(val, 1)}</td>
                            })}
                            <td key={"servos.foot.current.sum"} className="sum">{
                                Numbers.roundToFixed(this.state.currentSumAll, 1)
                            }</td>
                            <td  key={"servos.foot.voltage"} colSpan={4}>Mean</td>
                            {this.state.temperatureMeanJoints.map((val, i) => {
                                return <td key={"servos.foot.temperature"+i} >{Numbers.roundToFixed(val, 1)}</td>
                            })}
                            <td key={"servos.foot.temperature.mean"} className="sum">{
                                Numbers.roundToFixed(this.state.temperatureMeanAll, 1)}
                            </td>
                        </tr>
                    </tbody>
                </table>
                <table>
                    <thead>
                        <tr key={"state.h1"}>
                            <th key={"state.h1.leg"} colSpan={2}>Leg</th>
                            <th key={"state.h1.statusError"} colSpan={4}>StatusError (hex)</th>
                            <th key={"state.h1.statusDetail"} colSpan={4}>StatusDetail (hex)</th>
                            <th key={"state.h1.pwm"} colSpan={4}>PWM</th>
                            <th key={"sensors.h1.long"} colSpan={2}>Sensors</th>
                        </tr>
                        <tr key={"state.h2"}>
                            <th key={"state.h2.id"}>ID</th>
                            <th key={"state.h2.name"}>Name</th>
                            {this.jointNamesShort.map((x, i) => {
                                return <th className="statusError" key={"state.h2.statusError"+i}>{x}</th>
                            })}
                            {this.jointNamesShort.map((x, i) => {
                                return <th className="statusDetail" key={"state.h2.statusDetail"+i}>{x}</th>
                            })}
                            {this.jointNamesShort.map((x, i) => {
                                return <th className="statusDetail" key={"state.h2.pwm"+i}>{x}</th>
                            })}
                            <th key={"sensors.h2.distance"}>Distance (mm)</th>
                            <th key={"sensors.h2.weight"}>Weight (kg)</th>
                        </tr>
                    </thead>
                    <tbody>
                        {this.state.legs.map((leg, i) => {
                            return (
                                <tr key={"state.leg" + i}>
                                    <td className="leg" key={"state.leg.id"+i}>{leg.id}</td>
                                    <td className="leg" key={"state.leg.name"+i}>{leg.name}</td>
                                    {leg.joints.map((joint, j) => {
                                        return <td key={"state.leg."+i + ".statusError"+j}>{
                                            Strings.toHexString(joint.statusError, 2, true)
                                        }</td>
                                    })}
                                    {leg.joints.map((joint, j) => {
                                        return <td key={"state.leg."+i + ".statusDetail"+j}>{
                                            Strings.toHexString(joint.statusDetail, 2, true)
                                        }</td>
                                    })}
                                    {leg.joints.map((joint, j) => {
                                        return <td key={"state.leg."+i + ".pwm"+j}>{
                                            Numbers.roundToFixed(joint.pwm, 1)
                                        }</td>
                                    })}
                                    <td key={"sensors.leg.id"+i+".distance"}>{
                                        Numbers.roundToFixed(leg.distance, 1)
                                    }</td>
                                    <td key={"sensors.leg.id"+i+".weight"}>{
                                        Numbers.roundToFixed(leg.weight, 3)
                                    }</td>
                                </tr>
                            )
                        })}
                    </tbody>
                </table>
                <table>
                    <thead>
                        <tr key={"imu.h1"}>
                            <th key="imu.h1.acc" colSpan={3}>IMU Acceleration (m/s²)</th>
                            <th key="imu.h1.gyro" colSpan={3}>IMU Gryoscope (?)</th>
                        </tr>
                        <tr key={"imu.h2"}>
                            <td key="imu.h2.acc.x" >X</td>
                            <td key="imu.h2.acc.y" >Y</td>
                            <td key="imu.h2.acc.z" >Z</td>
                            <td key="imu.h2.gyro.x" >X</td>
                            <td key="imu.h2.gyro.y" >Y</td>
                            <td key="imu.h2.gyro.z" >Z</td>
                        </tr>
                    </thead>
                    <tbody>
                        <tr key={"imu.content"}>
                            {Array.from(this.state.imu.acceleration).map((val, i) => {
                                return <td key={"imu.acc"+i}>{
                                    Numbers.roundToFixed(val, 2)
                                }</td>
                            })}
                            {Array.from(this.state.imu.gyro).map((val, i) => {
                                return <td key={"imu.gyro"+i}>{
                                    Numbers.roundToFixed(val, 2)
                                }</td>
                            })}
                        </tr>
                    </tbody>
                </table>
            </div>
        );
    }
}