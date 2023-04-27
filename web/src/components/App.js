import React from "react";
import "./App.css";
import Robot3D from "./Robot3D"
import Main from "logic/Main"
import Time from "msl/time/Time"
import GamepadCanvas from "./GamepadCanvas";
import Overview from "./Overview";

export default class App extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            view: 0,
        }
    }

    componentDidMount(){
        
    }

    handleClick = e => {
        this.setState({
            view: (this.state.view+1)%3
        });
        // this.setState(calculate(this.state, buttonName));
    };

    handleClick2 = e => {
        var packet = Main.packetRecycler.popPacket("CS_GamepadJoystick");
        packet.pack("CS_GamepadJoystick", {
            key: 0xF0, state: 0, x: 0.42, y: 13.37
        });
        Main.packetSender.addPacket(packet);
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
                <div>
                    <button onClick={this.handleClick}>State view: {this.state.view}</button>
                </div>
                <div>
                    <button onClick={this.handleClick2}>Send event</button>   
                </div>
                <div>{view}</div>
            </div>
        );
    }
}