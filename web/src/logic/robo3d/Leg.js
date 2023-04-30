import Foot from "./Foot"
import Joint from "./Joint"
import * as THREE from "three";

export default class Leg {
    constructor(robo3d, robot, parent){
        this.robo3 = robo3d
        this.root = new THREE.Group()
        this.parent = parent
        this.robot = robot
        this.joints = []
        var nextJointParent = this.root
        for(var i = 0; i < 4; i++){
            var joint = new Joint(robo3d, this, nextJointParent)
            this.joints.push(joint)
            nextJointParent = joint.tail
        }
        this.foot = new Foot(robo3d, this, nextJointParent)
        this.parent.add(this.root)
    }
    setJointLengths(lengths){
        for(var i = 0; i < lengths; i++){
            this.joints[i].setLimbLength(lengths[i])
        }
    }
    setJointSizes(sizes){
        for(var i = 0; i < 4; i++){
            this.joints[i].setJointSize(sizes[i])
        }
    }
    setHipTranslation(translation){
        this.hipTranslation = translation
        this.root.position.copy(this.hipTranslation)
    }
    setHipRotation(angle){
        this.hipRotation = new THREE.Quaternion().setFromAxisAngle(new THREE.Vector3(0,1,0), angle)
        this.root.setRotationFromQuaternion(this.hipRotation)
    }
    update(canvas){
        for(var joint of this.joints){
            joint.update(canvas)
        }
        this.foot.update(canvas)
    }
    enableLayer(layer){
        // for(var joint of this.joints){
        //     joint.enableLayer(layer)
        // }
        this.foot.enableLayer(layer)
    }
}
