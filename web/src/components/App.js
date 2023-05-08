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
import CalibComponent from "./CalibComponent";
import StatusComponent from "./StatusComponent";
import ConsoleComponent from "./ConsoleComponent";
import Navbar from "./Navbar";

export default class App extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            view: 4,
            connected: false,
        }
    }

    componentDidMount(){
        Main.connection.subscribe("open", this, this.onConnectionOpen)
        Main.connection.subscribe("close", this, this.onConnectionClose)
    }

    componentWillUnmount(){
        Main.connection.unsubscribe("open", this)
        Main.connection.unsubscribe("close", this)
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
            view: (this.state.view+1)%5
        });
        // this.setState(calculate(this.state, buttonName));
    };

    handleClick2 = e => {
        var message = new PacketMessage("test")
        message.addString("s1", "fdsa")
        message.addFloat("f1", 0.000000000003434934)
        Main.addPacketMessage(message)
    };

    onSelectView = (view) => {
        this.setState({
            view: view,
        })
    }

    render() {
        var view;
        if(this.state.view == 0){
            view = <Overview />
        } else if(this.state.view == 1){
            view = <Robo3DComponent />
        } else if(this.state.view == 2){
            view = <GamepadComponent />
        } else if(this.state.view == 3){
            view = <CalibComponent />
        } else {
            view = <ConsoleComponent />
        }
        return (
            <div className="wrapper">
                <div className="main-container">
                    <div className="toprow">
                        <StatusComponent /> 
                        <Navbar onSelectView={this.onSelectView} view={this.state.view} />
                    </div>
                    <div className="view-container">{view}</div>
                </div>
                <div className="blocker">
                    <img src="img/disconnected.svg" />
                </div>
            </div>
        );
    }
}