import React from "react";
import "./App.css";
import Robot3D from "./Robot3D"
import Main from "logic/Main"
import Time from "msl/time/Time"
import GamepadCanvas from "./GamepadCanvas";
import Overview from "./Overview";
import PacketMessage from "logic/PacketMessage";
import $ from "jquery"
import Numbers from "msl/util/Numbers";

export default class App extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            view: 0,
            connected: false,
            status: {
                time: 0,
                ups: 0   ,
                fixedUps: 0,
                cap: 0,
                sleep: 0,
                clients: 0,
                maxDt: 0,
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
                cap: pm.getFloat("cap"),
                sleep: pm.getFloat("sleep"),
                clients: pm.getInt("clients"),
                maxDt: pm.getFloat("maxDt"),
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
            view = <Robot3D />
        } else {
            view = <GamepadCanvas />
        }
        return (
            <div>    
                <div className="blocker">
                    <img src="img/disconnected.svg" />
                </div>
                <div>
                    <button onClick={this.handleClick}>State view: {this.state.view}</button>
                    &nbsp;
                    <button onClick={this.handleClick2}>send test message</button>  
                    &nbsp;&nbsp; 
                    {Time.toBeautifulString(this.state.status.time, false, true, true, true, false)} | 
                    ups={Numbers.roundToFixed(this.state.status.ups, 1)} |
                    fixedUps={Numbers.roundToFixed(this.state.status.fixedUps, 1)} |
                    maxDt={Numbers.roundToFixed(this.state.status.maxDt, 2) + "ms"} |
                    cap={Numbers.roundToFixed(this.state.status.cap*100.0, 0) + "%"} |
                    sleep={Numbers.roundToFixed(this.state.status.sleep*100.0, 0) + "%"} |
                    clients={this.state.status.clients}
                </div>
                <div>{view}</div>
            </div>
        );
    }
}