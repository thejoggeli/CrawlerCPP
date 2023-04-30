import React from "react";
import "./Robo3DButton.css";
import Robo3D from "logic/robo3d/Robo3D";

export default class Robo3DButton extends React.Component {

    componentDidMount(){

    }

    componentWillUnmount(){

    }

    handleWindowResize = () => {

    }

    render(){
        var classNames = ["button"]
        if(this.props.active){
            classNames.push("active")
        }
        return (
            <div className={classNames.join(" ")} onClick={this.props.onClick}>
                <div className="inner">{this.props.display}</div>
            </div>
        )
    }

}