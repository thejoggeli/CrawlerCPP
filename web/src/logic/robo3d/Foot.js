import * as THREE from "three";

export default class Foot {
    constructor(leg, parent){
        
        this.leg = leg
        this.parent = parent

        var jointGeometry = new THREE.IcosahedronGeometry(1, 4)
        var jointMaterial = new THREE.MeshStandardMaterial({
            color: 0xC00000,
        });
        this.jointMesh = new THREE.Mesh(jointGeometry, jointMaterial)
        this.parent.add(this.jointMesh)
        
    }
    setJointSize(size){
        this.jointMesh.scale.setScalar(size)
    }
}
