import Numbers from "msl/util/Numbers";
import Leg from "./Leg";
import * as THREE from "three";

export default class Robot {

    constructor(robo3d){
        
        this.robo3 = robo3d
        this.root = new THREE.Group()
        this.legs = []

        // create legs
        for(var i = 0; i < 4; i++){
            var leg = new Leg(robo3d, this, this.root)
            this.legs.push(leg)
        }

        // body size
        this.bodySize = new THREE.Vector3(0.5, 0.25, 0.5)

        // set hip transform
        this.legs[0].setHipTranslation(new THREE.Vector3(-this.bodySize.x*0.5, 0, -this.bodySize.z*0.5))
        this.legs[1].setHipTranslation(new THREE.Vector3(-this.bodySize.x*0.5, 0, +this.bodySize.z*0.5))
        this.legs[2].setHipTranslation(new THREE.Vector3(+this.bodySize.x*0.5, 0, +this.bodySize.z*0.5))
        this.legs[3].setHipTranslation(new THREE.Vector3(+this.bodySize.x*0.5, 0, -this.bodySize.z*0.5))

        // set hip translation
        this.legs[0].setHipRotation(+90.0 * Numbers.deg2rad)
        this.legs[1].setHipRotation(+90.0 * Numbers.deg2rad)
        this.legs[2].setHipRotation(-90.0 * Numbers.deg2rad)
        this.legs[3].setHipRotation(-90.0 * Numbers.deg2rad)

        var jointSize = 0.08
        for(var i = 0; i < 4; i++){

            // set limb lengths
            this.legs[i].joints[0].setLimbLength(0.2)
            this.legs[i].joints[1].setLimbLength(0.2)
            this.legs[i].joints[2].setLimbLength(0.2)
            this.legs[i].joints[3].setLimbLength(0.2)

            // set limb sizes
            this.legs[i].joints[0].setLimbSize(0.04)
            this.legs[i].joints[1].setLimbSize(0.04)
            this.legs[i].joints[2].setLimbSize(0.04)
            this.legs[i].joints[3].setLimbSize(0.04)
            
            // set joints sizes
            this.legs[i].joints[0].setJointSize(jointSize)
            this.legs[i].joints[1].setJointSize(jointSize)
            this.legs[i].joints[2].setJointSize(jointSize)
            this.legs[i].joints[3].setJointSize(jointSize)
            this.legs[i].foot.setJointSize(jointSize)

            // set rotation axes
            this.legs[i].joints[0].rotationAxis = new THREE.Vector3(0,1,0)
            this.legs[i].joints[1].rotationAxis = new THREE.Vector3(-1,0,0)
            this.legs[i].joints[2].rotationAxis = new THREE.Vector3(-1,0,0)
            this.legs[i].joints[3].rotationAxis = new THREE.Vector3(-1,0,0)

        }

        // body mesh
        var bodyGeometry = new THREE.BoxGeometry(this.bodySize.x, this.bodySize.y, this.bodySize.z)
        var bodyMaterial = new THREE.MeshStandardMaterial({
            color: 0xC0C0C0,
        });
        this.bodyMesh = new THREE.Mesh(bodyGeometry, bodyMaterial)
        this.root.add(this.bodyMesh)

        // eye mesh
        var eyeGeometry = new THREE.IcosahedronGeometry(1, 3)
        var eyeMaterial = new THREE.MeshStandardMaterial({
            color: 0x404040,
            flatShading: true,
        });

        this.eyeMesh1 = new THREE.Mesh(eyeGeometry, eyeMaterial)
        this.eyeMesh1.scale.setScalar(this.bodySize.y*0.2)
        this.eyeMesh1.position.x = -this.bodySize.x*0.12
        this.eyeMesh1.position.z = -this.bodySize.z*0.5 + this.eyeMesh1.scale.x*0.25
        this.bodyMesh.add(this.eyeMesh1)

        this.eyeMesh2 = new THREE.Mesh(eyeGeometry, eyeMaterial)
        this.eyeMesh2.scale.setScalar(this.bodySize.y*0.2)
        this.eyeMesh2.position.x = +this.bodySize.x*0.12
        this.eyeMesh2.position.z = -this.bodySize.z*0.5 + this.eyeMesh2.scale.x*0.25
        this.bodyMesh.add(this.eyeMesh2)

        // set initial angles
        this.legs[0].joints[0].setAngle(+45.0 * Numbers.deg2rad)
        this.legs[1].joints[0].setAngle(-45.0 * Numbers.deg2rad)
        this.legs[2].joints[0].setAngle(+45.0 * Numbers.deg2rad)
        this.legs[3].joints[0].setAngle(-45.0 * Numbers.deg2rad)
        for(var i = 0; i < 4; i++){
            this.legs[i].joints[1].setAngle(30.0 * Numbers.deg2rad)
            this.legs[i].joints[2].setAngle(30.0 * Numbers.deg2rad)
            this.legs[i].joints[3].setAngle(30.0 * Numbers.deg2rad)
        }

        // body hover
        this.bodyMesh.onHoverStart = (mesh) => {
            mesh.material.oldColor = mesh.material.color.getHex()
            mesh.material.color.setRGB(1.0, 1.0, 0.0)
        }
        this.bodyMesh.onHoverEnd = (mesh) => {
            mesh.material.color.setHex(mesh.material.oldColor)
        }

    }
    update(canvas){
        for(var leg of this.legs){
            leg.update(canvas)
        }
    }
    enableLayer(layer){
        for(var leg of this.legs){
            leg.enableLayer(layer)
        }
        this.bodyMesh.layers.enable(layer)
    }
    setBodyScale(scale){
        this.bodyMesh.scale.setScalar(scale)
    }
    setBodyVisible(visible){
        this.bodyMesh.visible = visible
    }
    setEyesVisible(visible){
        this.eyeMesh1.visible = visible
        this.eyeMesh2.visible = visible
    }
    setBodyColor(color, opacity){
        if(opacity === undefined){
            opacity = 1.0
        }
        this.bodyMesh.material.transparent = opacity < 1.0
        this.bodyMesh.material.color = color
        this.bodyMesh.material.opacity = opacity
        this.bodyMesh.material.needsUpdate = true
    }
    setEyesColor(color, opacity){
        if(opacity === undefined){
            opacity = 1.0
        }
        this.eyeMesh1.material.transparent = opacity < 1.0
        this.eyeMesh1.material.color = color
        this.eyeMesh1.material.opacity = opacity
        this.eyeMesh1.material.needsUpdate = true
        this.eyeMesh2.material.transparent = opacity < 1.0
        this.eyeMesh2.material.color = color
        this.eyeMesh2.material.opacity = opacity
        this.eyeMesh2.material.needsUpdate = true
    }
    setBodyColor(color){
        this.bodyMesh.material.color = color
    }
    setEyesColor(color){
        this.eyeMesh1.material.color = color
        this.eyeMesh2.material.color = color
    }
}