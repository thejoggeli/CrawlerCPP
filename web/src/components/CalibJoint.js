import React from "react";
import "./CalibJoint.css";
import Numbers from "msl/util/Numbers";

export default class CalibJoint extends React.Component {

    constructor(props){
        super(props);
    }

    componentDidMount(){
    }

    componentWillUnmount(){
    }

    handleClick = (e, calibPointIdx) => {
        this.props.onCalibrate(this.props.joint, calibPointIdx)
    };

    render() {
        var joint = this.props.joint
        return (
            <tr className="joint">
                <td className="joint-name">{joint.name}</td>
                <td className="joint-angle" key={"angle-deg"}>{Numbers.roundToFixed(Numbers.rad2deg*joint.measuredAngle, 1)}</td> 
                <td className="joint-angle" key={"angle-uint16"}>{joint.measuredXYZ}</td>
                {Array.from(joint.points).map((point, pointIdx) => {
                    const style = {
                        "opacity": point.visible ? 1 : 0,  
                    }
                    var title = ""
                    var deg = Numbers.rad2deg*point.calibAngle
                    var degstr = Numbers.roundToFixed(deg, 0) + "°"
                    var title = "calibrate servo at " + degstr
                    var status = ""
                    if(point.done){
                        status = "✓"
                    }
                    return (
                        <td className="calib-point" key={pointIdx} style={style}>
                            <span className="calib-angle">{degstr}</span> 
                            <input type="text" placeholder="uint16" value={point.value} readOnly title={title} />
                            <button title={title}
                                onClick={(e) => this.handleClick(e, pointIdx)}
                                disabled={point.calibMessageSent}
                            >Set</button>
                            <span className="calib-done">{status}</span>
                        </td>
                    )
                })} 
            </tr>
        );
    }
}
