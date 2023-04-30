import React from "react";
import "./Robo3DComponent.css";
import Robo3D from "logic/robo3d/Robo3D";

export default class Robo3DComponent extends React.Component {

    static robo3d = null

    componentDidMount(){
        this.robo3d = new Robo3D()
        this.robo3d.mount()
        this.mount.appendChild(this.robo3d.getDomElement());
    }

    componentWillUnmount(){
        this.robo3d.unmount()
    }

    handleWindowResize = () => {

    }

    render(){
        return (
            <div ref={mount => {this.mount = mount;}} />
        )
    }

}