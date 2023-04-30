import React from "react";
import "./Robo3DComponent.css";
import Robo3D from "logic/robo3d/Robo3D";
import Robo3DButton from "./Robo3DButton";

export default class Robo3DComponent extends React.Component {

    static robo3d = null
    
    constructor(){
        super()
        this.state = {}
    }

    componentDidMount(){
        this.robo3d = new Robo3D(document.getElementById("robo3d-container"))
        this.robo3d.mount()
        this.robo3d.resize()
        this.setState({
            shadowVisible: this.robo3d.isShadowVisible()
        })
    }

    componentWillUnmount(){
        this.robo3d.unmount()
    }

    handleWindowResize = () => {

    }

    handleClick = (event) => {

    }

    handleClickShadow = (event) => {
        this.robo3d.setShadowVisible(!this.robo3d.isShadowVisible())
        this.setState({
            shadowVisible: this.robo3d.isShadowVisible()
        })
    }

    render(){
        return (
            <div className="robo3d-wrap">
                <div className="robo3d-toolbar">
                    <Robo3DButton display="S" onClick={this.handleClickShadow} active={this.state.shadowVisible} />
                    <Robo3DButton display="T" onClick={this.handleClick} />
                </div>
                <div className="robo3d-container" id="robo3d-container" />
            </div>
        )
    }

}