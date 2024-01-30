
import React from "react";
import "./GamepadComponent.css";
import Main from "logic/Main";
import Gamepad from "logic/gamepad/Gamepad";
import "./Navbar.css";
import NavbarButton from "./NavbarButton";
import PacketMessage from "logic/PacketMessage";

export default class GamepadComponent extends React.Component {

    constructor(props){
        super();

        this.state = {
            mode: 3,
        }
    }

    componentDidMount(){

        var container = document.getElementById("gamepad-container")

        this.gamepad = new Gamepad();
        this.gamepad.install(container);

        this.gamepad.subscribe("onButtonPress", this, this.onButtonPress)
        this.gamepad.subscribe("onButtonRelease", this, this.onButtonRelease)
        this.gamepad.subscribe("onJoystickMove", this, this.onJoystickMove)
        
        // Main.events.subscribe("update", this, this.handleUpdate)
        Main.packetMessages.subscribe("getBrain", this, this.handleGetBrain)
        
        var msg = new PacketMessage("getBrain")
        Main.addPacketMessage(msg)
        
    }

    componentWillUnmount(){
        this.gamepad.uninstall();
        // Main.events.unsubscribe("update", this)
        Main.packetMessages.unsubscribe("getBrain", this)
    }

    handleGetBrain = (message) => {
        console.log(message)
        var mode = message.getString("brain")
        this.setState({mode: mode});
    }

    handleUpdate = () => {

    }
    
    onButtonPress(event){
        Main.addPacket("GamepadKey", {
            key: event.id,
            state: 0,
        })
    }
    
    onButtonRelease(event){
        Main.addPacket("GamepadKey", {
            key: event.id,
            state: 1,
        })
    }
    
    onJoystickMove(event){
        Main.addPacket("GamepadJoystick", {
            key: event.id,
            x: event.x,
            y: event.y,
        })
    }

    handleChange = (event) => {
        // console.log(event)
        var mode = event.target.value;
        this.setState({mode: mode});
        var msg = new PacketMessage("setBrain")
        msg.addString("name", mode)    
        Main.addPacketMessage(msg)
    }

    render(){
        return (
            <div id="gamepad-flexbox">
                <div id="gamepad-navbar" >
                    <select value={this.state.mode} onChange={this.handleChange}>
                        <option value={"walk"}>Selected Mode: Walk (walk) </option>
                        <option value={"surf-1"}>Selected Mode: Translate (surf-1)</option>
                        <option value={"surf-2"}>Selected Mode: Rotate (surf-2)</option>
                        <option value={"dance"}>Selected Mode: Dance (dance)</option>
                        <option value={"calib"}>Selected Mode: Calibrate (calib)</option>
                    </select>
                </div>
                <div id="gamepad-container"></div>
            </div>
        )
    }

}