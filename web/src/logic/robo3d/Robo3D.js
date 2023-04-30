
import * as THREE from "three";
import {OrbitControls} from "three/addons/controls/OrbitControls.js";
import Time from "msl/time/Time";
import Main from "logic/Main";
import Robot from "./Robot";
import Timer from "msl/time/Timer";

export default class Robo3D {
    constructor(){

        this.lastAngleUpdateTime = 0
        this.anglePacketUnderway = false

        this.rendererSize = new THREE.Vector2();

        this.scene = new THREE.Scene();

        this.renderer = new THREE.WebGLRenderer();
        // this.renderer.setSize(window.innerWidth, window.innerHeight);
        this.renderer.setSize(640, 480);

        this.renderer.getSize(this.rendererSize);
        this.camera = new THREE.PerspectiveCamera(75, this.rendererSize.width/this.rendererSize.height, 0.1);
        this.camera.position.x = -0.75;
        this.camera.position.y = +0.75;
        this.camera.position.z = -1.5;

        // var geometry = new THREE.BoxGeometry(1,1,1);
        // var material = new THREE.MeshStandardMaterial({
        //     color: 0xFF00FF,
        // });
        // this.cube = new THREE.Mesh(geometry, material);
        // this.scene.add(this.cube);

        this.cameraControls = new OrbitControls(this.camera, this.renderer.domElement);
        this.cameraControls.target = new THREE.Vector3(0,0,0)

        this.directionalLight = new THREE.DirectionalLight(0xffffff, 1.0);
        this.scene.add(this.directionalLight);

        this.robot = new Robot()
        this.scene.add(this.robot.root)

    }

    mount(){
        Main.events.subscribe("update", this, this.update);
        Main.events.subscribe("resize", this, this.resize);
        Main.packetReceiver.subscribe("RespondLegAngles", this, this.handleRespondLegAngles)
    }

    unmount(){
        Main.events.unsubscribe("update", this);
        Main.events.unsubscribe("resize", this);
        Main.packetReceiver.unsubscribe("RespondLegAngles", this)
    }

    getDomElement(){
        return this.renderer.domElement
    }

    resize(){
        // this.renderer.setSize(window.innerWidth, window.innerHeight);
        // this.renderer.getSize(this.rendererSize);
        // this.camera.aspect = this.rendererSize.width/this.rendererSize.height;
        // this.camera.updateProjectionMatrix();
    }

    update(){

        if(Time.currentTime - this.lastAngleUpdateTime > 0.02 && !this.anglePacketUnderway){
            this.lastAngleUpdateTime = Time.currentTime
            Main.addPacket("RequestLegAngles", {
                "flags": 0xFFFFFFFF,
                "legIds": [0, 1, 2, 3],
            })
            this.anglePacketUnderway = true
        }

        this.directionalLight.position.copy(this.camera.position);
        this.directionalLight.lookAt(this.robot.root.position);
        
        this.renderer.render(this.scene, this.camera);

    }

    handleRespondLegAngles(packet){
        this.anglePacketUnderway = false
        var data = packet.data
        for(var leg of data.legs){
            for(var j in leg.joints){
                this.robot.legs[leg.id].joints[j].setAngle(leg.joints[j].targetAngle)
            }
        }
    }

}