
import React from "react";
import "./GamepadCanvas.css";
import Main from "logic/Main";
import Gamepad from "logic/gamepad/Gamepad";

export default class GamepadCanvas extends React.Component {


    componentDidMount(){

        var container = document.getElementById("gamepad-container")

        this.gamepad = new Gamepad();
        this.gamepad.install(container);

        this.gamepad.canvas.$element.width(900)
        this.gamepad.canvas.$element.height(450)

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

    render(){
        return (
            <div id="gamepad-container"></div>
        )
    }

}