
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

    }

    componentWillUnmount(){
        this.gamepad.uninstall();

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
        if(mode == 0){
            msg.addString("name", "walk")
        } else if(mode == 1){
            msg.addString("name", "surf-1")            
        } else if(mode == 2){
            msg.addString("name", "surf-2")            
        } else if(mode == 3){
            msg.addString("name", "dance")            
        } else if(mode == 4){
            msg.addString("name", "calib")            
        }
        Main.addPacketMessage(msg)
    }

    render(){
        return (
            <div id="gamepad-flexbox">
                <div id="gamepad-navbar" >
                    <select value={this.state.mode} onChange={this.handleChange}>
                        <option value={0}>Selected Mode: Walk</option>
                        <option value={1}>Selected Mode: Translate</option>
                        <option value={2}>Selected Mode: Rotate</option>
                        <option value={3}>Selected Mode: Dance</option>
                        <option value={4}>Selected Mode: Calibrate</option>
                    </select>
                </div>
                <div id="gamepad-container"></div>
            </div>
        )
    }

}