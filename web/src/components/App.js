import React from "react";
import "./App.css";
import Robot3D from "./Robot3D"
import Main from "logic/Main"
import Time from "msl/time/Time"
import GamepadCanvas from "./GamepadCanvas";
import Overview from "./Overview";
import PacketMessage from "logic/PacketMessage";

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
                <div>
                    <button onClick={this.handleClick}>State view: {this.state.view}</button>
                    &nbsp;
                    <button onClick={this.handleClick2}>send test message</button>   
                </div>
                <div>{view}</div>
            </div>
        );
    }
}