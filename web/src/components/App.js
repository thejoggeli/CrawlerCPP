import React from "react";
import "./App.css";
import Robo3DComponent from "./Robo3DComponent"
import Main from "logic/Main"
import Time from "msl/time/Time"
import GamepadComponent from "./GamepadComponent";
import Overview from "./Overview";
import PacketMessage from "logic/PacketMessage";
import $ from "jquery"
import Numbers from "msl/util/Numbers";
import StatusEntry from "./StatusEntry";

export default class App extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            view: 1,
            connected: false,
            status: {
                time: 0,
                ups: 0   ,
                fixedUps: 0,
                capFX: 0,
                capST: 0,
                sleep: 0,
                clients: 0,
            }
        }
    }

    componentDidMount(){
        Main.connection.subscribe("open", this, this.onConnectionOpen)
        Main.connection.subscribe("close", this, this.onConnectionClose)
        Main.packetMessages.subscribe("status", this, this.onPacketMessageStatus)
    }

    componentWillUnmount(){
        Main.connection.unsubscribe("open", this)
        Main.connection.unsubscribe("close", this)
        Main.packetMessages.unsubscribe("status", this, this.onPacketMessageStatus)
    }

    onPacketMessageStatus(pm){
        this.setState({
            status: {
                time: pm.getFloat("time"),
                ups: pm.getFloat("ups")   ,
                fixedUps: pm.getFloat("fixedUps"),
                capFX: pm.getFloat("capFX"),
                capST: pm.getFloat("capST"),
                sleep: pm.getFloat("sleep"),
                clients: pm.getInt("clients"),
            }
        })
    }

    onConnectionOpen(){
        this.setState({
            connected: true,
        })
        $(".blocker").fadeOut()
    }

    onConnectionClose(){
        this.setState({
            connected: false,
        })
        $(".blocker").fadeIn()
    }

    handleClick = e => {
        this.setState({
            view: (this.state.view+1)%3
        });
        // this.setState(calculate(this.state, buttonName));
    };

    handleClick2 = e => {
        var message = new PacketMessage("test")
        message.addString("s1", "fdsa")
        message.addFloat("f1", 0.000000000003434934)
        Main.addPacketMessage(message)
    };

    render() {
        var view;
        if(this.state.view == 0){
            view = <Overview />
        } else if(this.state.view == 1){
            view = <Robo3DComponent />
        } else {
            view = <GamepadComponent />
        }
        return (
            <div className="wrapper">
                <div className="blocker">
                    <img src="img/disconnected.svg" />
                </div>
                <div className="main-container">
                    <div className="toprow">
                        <button onClick={this.handleClick}>State view: {this.state.view}</button>
                        &nbsp;
                        <button onClick={this.handleClick2}>send test message</button>  
                        &nbsp;&nbsp; 
                        <StatusEntry name="Web" value={Time.toBeautifulString(Time.currentTime, false, true, true, true, false)} /> 
                        <StatusEntry name="Robot" value={Time.toBeautifulString(this.state.status.time, false, true, true, true, false)} />
                        <StatusEntry name="UPS" value={Numbers.roundToFixed(this.state.status.ups, 1)} />
                        <StatusEntry name="FixedUPS" value={Numbers.roundToFixed(this.state.status.fixedUps, 1)} />
                        <StatusEntry name="CapFX" value={Numbers.roundToFixed(this.state.status.capFX*100.0, 0) + "%"} />
                        <StatusEntry name="CapST" value={Numbers.roundToFixed(this.state.status.capST*100.0, 0) + "%"} />
                        <StatusEntry name="Sleep" value={Numbers.roundToFixed(this.state.status.sleep*100.0, 0) + "%"} />
                        <StatusEntry name="Clients" value={this.state.status.clients} last={true} />
                    </div>
                    <div className="view-container">{view}</div>
                </div>
            </div>
        );
    }
}