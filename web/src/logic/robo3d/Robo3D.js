
import * as THREE from "three";
import {OrbitControls} from "three/addons/controls/OrbitControls.js";
import Time from "msl/time/Time";
import Main from "logic/Main";
import Robot from "./Robot";
import Timer from "msl/time/Timer";
import Arrays from "msl/util/Arrays";
import Gfw from "msl/gfw/Gfw";
import Log from "msl/log/Log";

export default class Robo3D {
    constructor(container){

        this.canvas = Gfw.createCanvas("robo3d", {
			size: 100,
            container: container,
            type: "three",
        })

        this.hoverObjects = []

        this.mouseNormalized = new THREE.Vector2(-1,1)

        this.lastAngleUpdateTime = 0
        this.anglePacketUnderway = false

        this.rendererSize = new THREE.Vector2();

        this.scene = new THREE.Scene();

        this.renderer = new THREE.WebGLRenderer({
            canvas: this.canvas.element
        })
        // this.renderer.setSize(window.innerWidth, window.innerHeight);
        // this.renderer.setSize(640, 480); 

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

        this.cameraControls = new OrbitControls(this.camera, this.canvas.input.$overlay[0]);
        this.cameraControls.target = new THREE.Vector3(0,0,0)
        this.cameraControls.addEventListener('start', this.onCameraControlsStart);
        this.cameraControls.addEventListener('end', this.onCameraControlsEnd);
        this.cameraControlsActive = false

        this.directionalLight = new THREE.DirectionalLight(0xffffff, 1.0);
        this.scene.add(this.directionalLight);

        this.robot = new Robot(this)
        this.robot.enableLayer(2)
        this.scene.add(this.robot.root)

        this.measuredRobot = new Robot(this)
        this.measuredRobot.enableLayer(3)
        // this.measuredRobot.setBodyVisible(false)
        this.scene.add(this.measuredRobot.root)

        this.dragCounter = 0

        var opacity = 0.5
        var red = 1.0
        var green = 0.5
        var blue = 0.5
        var downscale = 0.95
        this.measuredRobot.setBodyColor(new THREE.Color(red, green, blue), opacity)
        this.measuredRobot.setEyesColor(new THREE.Color(red, green, blue), opacity)
        this.measuredRobot.setBodyScale(downscale)
        for(var i = 0; i < 4; i++){
            for(var j = 0; j < 4; j++){
                var joint = this.measuredRobot.legs[i].joints[j]
                joint.setJointColor(new THREE.Color(red, green, blue), opacity)
                joint.setLimbColor(new THREE.Color(red, green, blue), opacity)
                joint.setJointSize(joint.getJointSize()*downscale)
                joint.setLimbSize(joint.getLimbSize()*downscale)
            }
            var foot = this.measuredRobot.legs[i].foot
            foot.setJointColor(new THREE.Color(red, green, blue), opacity)
            foot.setJointSize(foot.getJointSize()*downscale)
        }

    }

    setMeasuredVisible(visible){
        this.measuredRobot.root.visible = visible
    }

    isMeasuredVisible(){
        return this.measuredRobot.root.visible
    }

    setTargetVisible(visible){
        this.robot.root.visible = visible
    }

    isTargetVisible(){
        return this.robot.root.visible
    }

    mount(){
        Main.events.subscribe("update", this, this.update);
        this.canvas.subscribe("resize", this, this.resize); // resize after canvas has resized itself
        Main.messenger.subscribe("footDragStart", this, this.onFootDragStart)
        Main.messenger.subscribe("footDragStop", this, this.onFootDragStop)
        Main.packetReceiver.subscribe("RespondLegAngles", this, this.handleRespondLegAngles)
    }

    unmount(){
        Main.events.unsubscribe("update", this);
        this.canvas .unsubscribe("resize", this);
        Main.messenger.unsubscribe("footDragStart", this)
        Main.messenger.unsubscribe("footDragStop", this)
        Main.packetReceiver.unsubscribe("RespondLegAngles", this)
        Gfw.destroyCanvas(this.canvas.id)
    }

    onCameraControlsStart = () => {
        this.cameraControlsActive = true
    }

    onCameraControlsEnd = () => {
        this.cameraControlsActive = false
    }

    onFootDragStart(){
        this.dragCounter += 1
    }

    onFootDragStop(){
        this.dragCounter -= 1
        if(this.dragCounter < 0){
            Log.error("Robo3D", "dragCounter is " + this.dragCounter)
        }
    }

    resize(){
        var w = this.canvas.element.width
        var h = this.canvas.element.height
        this.renderer.setSize(w, h);
        this.renderer.getSize(this.rendererSize);
        this.camera.aspect = w/h;
        this.camera.updateProjectionMatrix();
    }

    update(){

        var input = this.canvas.input

        // normalized mouse position
        this.mouseNormalized.x = +(input.mouse.relativePosition.x / this.canvas.element.width) * 2 - 1;
        this.mouseNormalized.y = -(input.mouse.relativePosition.y / this.canvas.element.height) * 2 + 1;
        
        // request new angles from server
        if(Time.currentTime - this.lastAngleUpdateTime > 0.02 && !this.anglePacketUnderway){
            this.lastAngleUpdateTime = Time.currentTime
            Main.addPacket("RequestLegAngles", {
                "flags": 0xFFFFFFFF,
                "legIds": [0, 1, 2, 3],
            })
            this.anglePacketUnderway = true
        }

        // set light position and rotation
        this.directionalLight.position.copy(this.camera.position);
        this.directionalLight.lookAt(this.robot.root.position);

        // mouse intersection
        for(var obj of this.hoverObjects){
            obj.hoverStay = false
            obj.hoverStart = false
            obj.hoverEnd = false
        }
        if(!this.cameraControlsActive && this.dragCounter == 0){
            var raycaster = new THREE.Raycaster();
            raycaster.layers.set(2)
            raycaster.setFromCamera(this.mouseNormalized, this.camera);
            var intersects = raycaster.intersectObject(this.robot.root, true);
            for(var inter of intersects){
                var obj = inter.object
                if(!obj.hover){
                    obj.hoverStart = true
                    obj.hover = true
                    if(obj.onHoverStart){
                        obj.onHoverStart(obj)
                    }
                    this.hoverObjects.push(obj)
                }
                obj.hoverStay = true
                break
            }
        }
        for(var obj of this.hoverObjects){
            if(!obj.hoverStay){
                obj.hover = false
                obj.hoverEnd = true
                if(obj.onHoverEnd){
                    obj.onHoverEnd(obj)
                }
                Arrays.removeObject(this.hoverObjects, obj)
            }
        }

        if(this.hoverObjects.length > 0){
            this.cameraControls.enabled = false
        } else {
            this.cameraControls.enabled = true
        }

        // update robot
        this.robot.update(this.canvas)

        // render the scene
        this.renderer.render(this.scene, this.camera);

    }

    handleRespondLegAngles(packet){
        this.anglePacketUnderway = false
        var data = packet.data
        for(var leg of data.legs){
            for(var j in leg.joints){
                this.robot.legs[leg.id].joints[j].setAngle(leg.joints[j].targetAngle)
                this.measuredRobot.legs[leg.id].joints[j].setAngle(leg.joints[j].measuredAngle)
            }
        }
    }

}