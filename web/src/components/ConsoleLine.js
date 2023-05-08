
import React from "react";
import "./ConsoleLine.css";
import Main from "logic/Main";
import Time from "msl/time/Time";

export default class ConsoleLine extends React.Component {

    constructor(props){
        super(props)
        var line = this.props.line
        this.classes = ["console-line"]
        if(line.parsed){
            this.classes.push("parsed")
        }
        this.className = this.classes.join(" ")
        this.timestamp = Time.toTimestampString(line.timestamp/1000.0)
    }

    componentDidMount(){

    }

    componentWillUnmount(){

    }

    render(){
        var line = this.props.line
        return (
            <div className={this.className}>
                <div className="console-timestamp">[{this.timestamp}]&nbsp;</div>
                <div className="console-text">{line.text}</div>
            </div>
        )
    }

}