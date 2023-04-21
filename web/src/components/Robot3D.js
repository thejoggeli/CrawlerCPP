import React from "react";
import "./Robot3D.css";
import * as THREE from "three";
import {OrbitControls} from "three/addons/controls/OrbitControls.js";
import Time from "msl/time/Time";
import Main from "logic/Main";

export default class Robot3D extends React.Component {

    animationFrameRequest = null;
    rendererSize = new THREE.Vector2();

    componentDidMount(){

        this.scene = new THREE.Scene();

        this.renderer = new THREE.WebGLRenderer();
        // this.renderer.setSize(window.innerWidth, window.innerHeight);
        this.renderer.setSize(640, 480);

        this.mount.appendChild(this.renderer.domElement);

        this.renderer.getSize(this.rendererSize);
        this.camera = new THREE.PerspectiveCamera(75, this.rendererSize.width/this.rendererSize.height, 0.1);
        this.camera.position.z = 5;

        var geometry = new THREE.BoxGeometry(1,1,1);
        var material = new THREE.MeshStandardMaterial({
        // var material = new THREE.MeshBasicMaterial({
            color: 0xFF00FF,
        });
        this.cube = new THREE.Mesh(geometry, material);
        this.scene.add(this.cube);

        this.renderer.render(this.scene, this.camera);

        this.cameraControls = new OrbitControls(this.camera, this.renderer.domElement);
        this.cameraControls.target = this.cube.position;

        this.directionalLight = new THREE.DirectionalLight(0xffffff, 1.0);
        this.scene.add(this.directionalLight);

        Main.events.subscribe("update", this, this.update);

        window.addEventListener("resize", this.handleWindowResize);

    }

    componentWillUnmount(){
        window.removeEventListener("resize", this.handleWindowResize);
        Main.events.unsubscribe("update", this);
    }

    handleWindowResize = () => {
        console.log("Robot3D", "handleWindowResize()")
        // this.renderer.setSize(window.innerWidth, window.innerHeight);
        this.renderer.setSize(window.innerWidth, window.innerHeight);
        this.rendererSize = this.renderer.getSize();
        this.camera.aspect = this.rendererSize.width/this.rendererSize.height;
        this.camera.updateProjectionMatrix();
    }

    update = () => {
        console.log("Robot3D update");

        this.directionalLight.position.copy(this.camera.position);
        this.directionalLight.lookAt(this.cube.position);
        
        this.cube.rotation.x += 0.01;
        this.cube.rotation.y += 0.0117;
        this.renderer.render(this.scene, this.camera);

    }

    render(){
        return (
            <div ref={mount => {this.mount = mount;}} />
        )
    }

}