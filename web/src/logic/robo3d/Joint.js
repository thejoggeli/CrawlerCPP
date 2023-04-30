import * as THREE from "three";

export default class Joint {
    constructor(leg, parent){

        this.leg = leg
        this.parent = parent
        this.rotationAxis = new THREE.Vector3(1,0,0)

        this.length = 1
        this.root = new THREE.Group()
        this.tail = new THREE.Group()
        this.root.add(this.tail)
        this.parent.add(this.root)

        var jointGeometry = new THREE.IcosahedronGeometry(1, 3)
        var jointMaterial = new THREE.MeshStandardMaterial({
            color: 0xC00000,
            flatShading: true,
        });
        this.jointMesh = new THREE.Mesh(jointGeometry, jointMaterial)
        this.root.add(this.jointMesh)

        var limbGeometry = new THREE.CylinderGeometry(1, 1, 1)
        var limbMaterial = new THREE.MeshStandardMaterial({
            color: 0xC0C0C0,
            flatShading: true,
        });
        this.limbMesh = new THREE.Mesh(limbGeometry, limbMaterial)
        this.limbMesh.setRotationFromAxisAngle(new THREE.Vector3(1,0,0), Math.PI*0.5)
        this.root.add(this.limbMesh)
        
    }
    setLimbLength(length){
        this.length = length
        this.tail.position.z = -length
        this.limbMesh.scale.set(this.limbMesh.scale.x, length, this.limbMesh.scale.z)
        this.limbMesh.position.set(0, 0, -length*0.5)
    }
    setLimbSize(size){
        this.limbMesh.scale.set(size, this.limbMesh.scale.y, size)
    }
    setJointSize(size){
        this.jointMesh.scale.setScalar(size)
    }
    setAngle(angle){
        this.root.setRotationFromAxisAngle(this.rotationAxis, angle)
    }
}
