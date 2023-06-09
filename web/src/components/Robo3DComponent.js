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
            measuredVisible: this.robo3d.isMeasuredVisible(),
            targetVisible: this.robo3d.isTargetVisible(),
        })
    }

    componentWillUnmount(){
        this.robo3d.unmount()
    }

    handleWindowResize = () => {

    }

    handleClick = (event) => {

    }

    handleClickMeasured = (event) => {
        this.robo3d.setMeasuredVisible(!this.robo3d.isMeasuredVisible())
        this.setState({
            measuredVisible: this.robo3d.isMeasuredVisible()
        })
    }

    handleClickTarget = (event) => {
        this.robo3d.setTargetVisible(!this.robo3d.isTargetVisible())
        this.setState({
            targetVisible: this.robo3d.isTargetVisible()
        })
    }

    render(){
        return (
            <div className="robo3d-wrap">
                <div className="robo3d-toolbar">
                    <Robo3DButton display="T" onClick={this.handleClickTarget} active={this.state.targetVisible} />
                    <Robo3DButton display="M" onClick={this.handleClickMeasured} active={this.state.measuredVisible} />
                </div>
                <div className="robo3d-container" id="robo3d-container" />
            </div>
        )
    }

}