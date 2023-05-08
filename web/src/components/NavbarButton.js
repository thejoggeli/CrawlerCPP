import React from "react";
import "./NavbarButton.css";

export default class NavbarButton extends React.Component {

    constructor(props){
        super(props);
    }

    componentDidMount(){

    }

    componentWillUnmount(){

    }

    render() {
        var classes = ["navbar-button"]
        if(this.props.active){
            classes.push("active")
        }
        return (
            <div 
                className={classes.join(" ")} 
                onClick={() => this.props.onClick(this)}
            >{this.props.label}</div>
        );
    }
}