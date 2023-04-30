import Main from "logic/Main";
import Time from "msl/time/Time";
import * as THREE from "three";

export default class Foot {
    constructor(robo3d, leg, parent){
        
        this.robo3d = robo3d
        this.leg = leg
        this.parent = parent

        var jointGeometry = new THREE.IcosahedronGeometry(1, 3)
        var jointMaterial = new THREE.MeshStandardMaterial({
            // color: 0xC00000,
            color: 0xC0C0C0,
            flatShading: true,
        });
        this.jointMesh = new THREE.Mesh(jointGeometry, jointMaterial)
        this.jointMesh.layers.enable(2)
        this.parent.add(this.jointMesh)

        var dragGeometry = new THREE.IcosahedronGeometry(1, 3)
        var dragMaterial = new THREE.MeshStandardMaterial({
            color: 0xFFFF00,
            // transparent: true,
            // opacity: 0.8,
            flatShading: true,
        });
        this.dragMesh = new THREE.Mesh(dragGeometry, dragMaterial)
        this.dragging = false

        var dragPlaneGeometry = new THREE.PlaneGeometry(100, 100)
        var dragPlaneMaterial = new THREE.MeshStandardMaterial({
            color: 0x0000FF,
            transparent: true,
            opacity: 0.5,
            side: THREE.DoubleSide,
            flatShading: true,
        });
        this.dragPlaneMesh = new THREE.Mesh(dragPlaneGeometry, dragPlaneMaterial)
        this.dragPlaneMesh.layers.enable(7)
        this.dragPlaneMesh.visible = false
        this.dragOffset = new THREE.Vector3()

        var dragLineGeometry = new THREE.CylinderGeometry(0.02, 0.02, 1)
        var dragLineMaterialX = new THREE.MeshStandardMaterial({
            color: 0xFF8080,
            flatShading: true,
        });
        var dragLineMaterialY = new THREE.MeshStandardMaterial({
            color: 0x80FF80,
            flatShading: true,
        });
        var dragLineMaterialZ = new THREE.MeshStandardMaterial({
            color: 0x8080FF,
            flatShading: true,
        });
        this.dragLineX = new THREE.Mesh(dragLineGeometry, dragLineMaterialX)
        this.dragLineY = new THREE.Mesh(dragLineGeometry, dragLineMaterialY)
        this.dragLineZ = new THREE.Mesh(dragLineGeometry, dragLineMaterialZ)

        this.jointMesh.onHoverStart = this.onHoverStart
        this.jointMesh.onHoverEnd = this.onHoverEnd
        
    }
    onHoverStart = () => {
        var mesh = this.jointMesh
        mesh.material.oldColor = mesh.material.color.getHex()
        mesh.material.color.setRGB(1.0, 1.0, 0.0)
    }
    onHoverEnd = () => {
        var mesh = this.jointMesh
        mesh.material.color.setHex(mesh.material.oldColor)
    }
    setJointColor(color, opacity){
        if(opacity === undefined){
            opacity = 1.0
        }
        this.jointMesh.material.transparent = opacity < 1.0
        this.jointMesh.material.color = color
        this.jointMesh.material.opacity = opacity
        this.jointMesh.material.needsUpdate = true
    }
    getJointSize(){
        return this.jointMesh.scale.x
    }
    setJointSize(size){
        this.jointMesh.scale.setScalar(size)
        this.dragMesh.scale.setScalar(size)
    }
    getDragPlanePosition(out){
        var raycaster = new THREE.Raycaster();
        raycaster.layers.set(7)
        raycaster.setFromCamera(this.robo3d.mouseNormalized, this.robo3d.camera);
        var intersects = raycaster.intersectObject(this.dragPlaneMesh, true);
        if(intersects.length > 0){
            var inter = intersects[0]
            out.copy(inter.point)
            return true
        }
        return false
    }
    updateDragLines(){
        var x = this.dragMesh.position.x
        var y = this.dragMesh.position.y
        var z = this.dragMesh.position.z
        this.dragLineX.position.set(x*0.5, y, z)
        this.dragLineY.position.set(x, y*0.5, z)
        this.dragLineZ.position.set(x, y, z*0.5)
        this.dragLineX.scale.y = x
        this.dragLineY.scale.y = y
        this.dragLineZ.scale.y = z
    }
    update(canvas){
        if(canvas.input.mouseDown() && this.jointMesh.hover && !this.dragging){
            Main.messenger.sendMessage("footDragStart")
            this.dragging = true
            this.robo3d.scene.add(this.dragPlaneMesh)
            this.robo3d.scene.add(this.dragMesh)
            // this.robo3d.scene.add(this.dragLineX)
            // this.robo3d.scene.add(this.dragLineY)
            // this.robo3d.scene.add(this.dragLineZ)
            this.dragLineX.setRotationFromEuler(new THREE.Euler(0, 0, Math.PI*0.5))
            this.dragLineZ.setRotationFromEuler(new THREE.Euler(Math.PI*0.5, 0, 0))
            this.jointMesh.getWorldPosition(this.dragPlaneMesh.position)
            // this.dragPlaneMesh.setRotationFromAxisAngle(new THREE.Vector3(1,0,0), Math.PI*0.5)
            this.dragPlaneMesh.setRotationFromQuaternion(this.robo3d.camera.quaternion)
            this.dragPlaneMesh.updateMatrixWorld()
            this.getDragPlanePosition(this.dragOffset)
            this.dragOffset.sub(this.dragPlaneMesh.position)
            this.onHoverEnd()
            // this.updateDragLines()
        }
        if(this.dragging){
            this.getDragPlanePosition(this.dragMesh.position)
            this.dragMesh.position.sub(this.dragOffset)
            var len = this.dragOffset.length()
            // this.updateDragLines()
            if(len > 0.01){
                this.dragOffset.setLength(len - len * Time.deltaTime*4)
            }
        }
        if(this.dragging && canvas.input.mouseUp()){
            this.dragging = false
            this.robo3d.scene.remove(this.dragPlaneMesh)
            this.robo3d.scene.remove(this.dragMesh)
            this.robo3d.scene.remove(this.dragLineX)
            this.robo3d.scene.remove(this.dragLineY)
            this.robo3d.scene.remove(this.dragLineZ)
            Main.messenger.sendMessage("footDragStop")
        }
    }
    enableLayer(layer){
        this.jointMesh.layers.enable(layer)
    }
}
