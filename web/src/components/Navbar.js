import React from "react";
import "./Navbar.css";
import NavbarButton from "./NavbarButton";

export default class Navbar extends React.Component {

    constructor(props){
        super(props);
    }

    componentDidMount(){

    }

    componentWillUnmount(){

    }

    handleClick = (button) => {
        console.log(button)
        this.props.onSelectView(button.props.view)
    }

    render() {
        return (
            <div className="navbar">
                <NavbarButton label="Overview" onClick={this.handleClick} active={this.props.view == 0} view={0} />
                <NavbarButton label="Robo3D" onClick={this.handleClick} active={this.props.view == 1} view={1} />
                <NavbarButton label="Gamepad" onClick={this.handleClick} active={this.props.view == 2} view={2} />
                <NavbarButton label="Calibrate" onClick={this.handleClick} active={this.props.view == 3} view={3} />
                <NavbarButton label="Console" onClick={this.handleClick} active={this.props.view == 4} view={4} />
            </div>
        );
    }
}