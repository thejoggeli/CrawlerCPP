import React from "react";
import "./CalibComponent.css";
import CalibJoint from "./CalibJoint";

export default class CalibComponent extends React.Component {

    constructor(props){
        super(props);
        this.state = {

        }
        this.legs = []
        var legNames = ["Front-Left", "Back-Left", "Back-Right", "Front-Right"]
        var jointNames = ["H0", "K1", "K2", "K3"]
        for(var i = 0; i < 4; i++){
            var leg = {joints: [], index: i, name: legNames[i]}
            for(var j = 0; j < 4; j++){
                var joint = {
                    leg: leg,
                    id: i*4+j,
                    index: j,
                    name: jointNames[j],
                }
                leg.joints.push(joint)
            }
            this.legs.push(leg)
        }
    }

    componentDidMount(){
    }

    componentWillUnmount(){
    }

    handleClick = e => {
    };

    render() {
        return (
            <div className="calib">
                {Array.from(this.legs).map((leg, legIdx) => {
                    var jointComponents = Array.from(leg.joints).map((joint, jointIdx) => {
                        return <CalibJoint joint={joint} key={joint.id} />
                    })
                    return (
                        <div key={leg.index} className="leg">
                            <div className="leg-name">{leg.name} ({leg.index})</div>
                            <table className="joints">
                                <thead>
                                    <tr>
                                        <th className="joint-name" key={"name"}>Joint</th>
                                        <th className="joint-name" key={0}>-90°</th>
                                        <th className="joint-name" key={1}>0°</th>
                                        <th className="joint-name" key={2}>°90°</th>
                                    </tr>
                                </thead>
                                <tbody>
                                    {jointComponents}
                                </tbody>
                            </table>
                        </div>
                    )
                })}
            </div>
        );
    }
}