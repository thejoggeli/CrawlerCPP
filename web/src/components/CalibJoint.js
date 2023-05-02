import React from "react";
import "./CalibJoint.css";
import Numbers from "msl/util/Numbers";

export default class CalibJoint extends React.Component {

    constructor(props){
        super(props);
        this.state = {
            points: [
                {uint16: Numbers.randomInt(400, 600), done: false},
                {uint16: Numbers.randomInt(400, 600), done: false},
                {uint16: Numbers.randomInt(400, 600), done: false},
            ]
        }
    }

    componentDidMount(){
    }

    componentWillUnmount(){
    }

    handleClick = (e, calibPointIdx) => {
        this.state.points[calibPointIdx].done = true
        this.setState(this.state)
    };

    render() {
        var joint = this.props.joint
        var leg = joint.leg
        var calibPoints = [
            {angle: -90, visible: true},
            {angle: 0, visible: true},
            {angle: 90, visible: true},   
        ]
        if(joint.index == 0){
            if(leg.index == 0 || leg.index == 2){
                calibPoints[2].visible = false
            } else {
                calibPoints[0].visible = false
            }
        }
        return (
            <tr className="joint">
                <td className="joint-name">{joint.name}</td>
                {Array.from(calibPoints).map((calibPoint, calibPointIdx) => {
                    const style = {
                        // "visibility": calibPoint.visible ? "visible" : "hidden",
                        "opacity": calibPoint.visible ? 1.0 : 0.25,
                    }
                    var disabled = calibPoint.visible ? false : true
                    var point = this.state.points[calibPointIdx]
                    return (
                        <td className="calib-point" key={calibPointIdx} style={style}> 
                            <input disabled={disabled} type="text" placeholder="uint16" value={point.uint16} readOnly />
                            <button disabled={disabled}
                                onClick={(e) => this.handleClick(e, calibPointIdx)}
                            >Set</button>
                            <span className="calib-done">{point.done ? "✓" : ""}</span>
                        </td>
                    )
                })} 
            </tr>
        );
    }
}
