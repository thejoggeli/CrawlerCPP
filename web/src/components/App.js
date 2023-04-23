import React from "react";
import "./App.css";
import Robot3D from "./Robot3D"
import Main from "logic/Main"
import Time from "msl/time/Time"
import GamepadCanvas from "./GamepadCanvas";

export default class App extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            showRobot3D: false,
        }
    }

    componentDidMount(){
        
    }

    handleClick = e => {
        this.setState({showRobot3D: !this.state.showRobot3D});
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
        var toggleViewButton;
        var robot;
        var gamepad;
        if(this.state.showRobot3D){
            toggleViewButton = <button onClick={this.handleClick}>Click me</button>
            robot = <Robot3D />
        } else {
            toggleViewButton = <button onClick={this.handleClick}>Click me again</button>   
            gamepad = <GamepadCanvas />
        }
        var sendEventButton = <button onClick={this.handleClick2}>Send event</button>   
        return (
            <div>    
                <div>{toggleViewButton}</div>
                <div>{sendEventButton}</div>
                <div>{robot}</div>
                <div>{gamepad}</div>
            </div>
        );
    }
}