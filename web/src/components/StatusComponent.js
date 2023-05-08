import React from "react";
import "./StatusComponent.css";
import Robo3DComponent from "./Robo3DComponent"
import Main from "logic/Main"
import Time from "msl/time/Time"
import GamepadComponent from "./GamepadComponent";
import Overview from "./Overview";
import PacketMessage from "logic/PacketMessage";
import $ from "jquery"
import Numbers from "msl/util/Numbers";
import StatusEntry from "./StatusEntry";
import CalibComponent from "./CalibComponent";

export default class StatusComponent extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            status: {
                time: 0,
                ups: 0   ,
                fixedUps: 0,
                capFR: 0,
                capST: 0,
                maxFR: 0,
                maxST: 0,
                sleep: 0,
                clients: 0,
            }
        }
    }

    componentDidMount(){
        Main.packetMessages.subscribe("status", this, this.onPacketMessageStatus)
    }

    componentWillUnmount(){
        Main.packetMessages.unsubscribe("status", this, this.onPacketMessageStatus)
    }

    onPacketMessageStatus(pm){
        this.setState({
            status: {
                time: pm.getFloat("time"),
                ups: pm.getFloat("ups")   ,
                fixedUps: pm.getFloat("fixedUps"),
                capFR: pm.getFloat("capFR"),
                capST: pm.getFloat("capST"),
                maxFR: pm.getFloat("maxFR"),
                maxST: pm.getFloat("maxST"),
                sleep: pm.getFloat("sleep"),
                clients: pm.getInt("clients"),
            }
        })
    }

    render() {
        return (
            <div className="status-component">
                <StatusEntry name="Web" value={Time.toBeautifulString(Time.currentTime, false, true, true, true, false)} /> 
                <StatusEntry name="Robot" value={Time.toBeautifulString(this.state.status.time, false, true, true, true, false)} />
                <StatusEntry name="UPS" value={Numbers.roundToFixed(this.state.status.ups, 1)} />
                <StatusEntry name="FixedUPS" value={Numbers.roundToFixed(this.state.status.fixedUps, 1)} />
                <StatusEntry name="CapFR" value={Numbers.roundToFixed(this.state.status.capFR*100.0, 0) + "%"} />
                <StatusEntry name="CapST" value={Numbers.roundToFixed(this.state.status.capST*100.0, 0) + "%"} />
                <StatusEntry name="MaxFR" value={Numbers.roundToFixed(this.state.status.maxFR, 2) + "ms"}/>
                <StatusEntry name="MaxST" value={Numbers.roundToFixed(this.state.status.maxST, 2) + "ms"}/>
                <StatusEntry name="Sleep" value={Numbers.roundToFixed(this.state.status.sleep*100.0, 0) + "%"} />
                <StatusEntry name="Clients" value={this.state.status.clients} last={true} />
            </div>
        );
    }
}