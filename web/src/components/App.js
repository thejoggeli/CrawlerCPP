import React from "react";
import "./App.css";
import Robot3D from "./Robot3D"
import Main from "logic/Main"
import Time from "msl/time/Time"
import { useState } from 'react';
import Log from "msl/log/Log"
import LogTargetConsole from "msl/log/LogTargetConsole"

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
        requestAnimationFrame(this.frame.bind(this));
    }

    handleClick = e => {
        this.setState({showRobot3D: !this.state.showRobot3D});
        // this.setState(calculate(this.state, buttonName));
    };

    frame(){
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
        return (
            <div>    
                <div>{button}</div>
                <div>{robot}</div>
            </div>
        );
    }
}