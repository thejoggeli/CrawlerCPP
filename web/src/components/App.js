import React from "react";
import "./App.css";
import Robot3D from "./Robot3D"
import Main from "logic/Main"
import Time from "msl/time/Time"

export default class App extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            showRobot3D: true,
        }
    }

    componentDidMount(){
        Main.init();
        Time.start();
        requestAnimationFrame(this.frame);
    }

    handleClick = e => {
        this.setState({showRobot3D: !this.state.showRobot3D});
        // this.setState(calculate(this.state, buttonName));
    };

    handleClick2 = e => {
        var packet = Main.packetRecycler.popPacket("GamepadJoystick");
        packet.pack("GamepadJoystick", {
            key: 0xF0, state: 0, x: 0.42, y: 13.37
        });
        Main.packetSender.addPacket(packet);
    };

    frame = () => {
        Main.update();
        requestAnimationFrame(this.frame.bind(this));
    }

    render() {
        var button;
        var robot;
        if(this.state.showRobot3D){
            button = <button onClick={this.handleClick}>Click me</button>
            robot = <Robot3D />
        } else {
            button = <button onClick={this.handleClick}>Click me again</button>   
        }
        var button2 = <button onClick={this.handleClick2}>Send event</button>   
        return (
            <div>    
                <div>{button}</div>
                <div>{button2}</div>
                <div>{robot}</div>
            </div>
        );
    }
}