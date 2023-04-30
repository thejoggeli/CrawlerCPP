import Foot from "./Foot"
import Joint from "./Joint"
import * as THREE from "three";

export default class Leg {
    constructor(robot, parent){
        this.root = new THREE.Group()
        this.parent = parent
        this.robot = robot
        this.joints = []
        var nextJointParent = this.root
        for(var i = 0; i < 4; i++){
            var joint = new Joint(this, nextJointParent)
            this.joints.push(joint)
            nextJointParent = joint.tail
        }
        this.foot = new Foot(this, nextJointParent)
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
}
