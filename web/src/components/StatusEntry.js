import React from "react";
import "./StatusEntry.css";

export default class StatusEntry extends React.Component {

    constructor(props){
        super(props)
    }

    componentDidMount(){

    }

    componentWillUnmount(){

    }

    render() {
        var className = ["status-entry"]
        if(this.props.last){
            className.push("last")
        }
        return (
            <div className={className.join(" ")}>
                <div className="inner">
                    <span className="name">{this.props.name}=</span>
                    <span className="value">{this.props.value}</span>
                </div>
            </div>
        );
    }
}