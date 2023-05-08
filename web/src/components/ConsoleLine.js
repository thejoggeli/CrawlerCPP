
import React from "react";
import "./ConsoleLine.css";
import Main from "logic/Main";
import Time from "msl/time/Time";
import Console from "logic/Console";

export default class ConsoleLine extends React.Component {

    constructor(props){
        super(props)
        var line = this.props.line
        this.classes = ["console-line"]
        if(line.type == Console.LineTypeCommand){
            this.classes.push("command")
        } else if(line.type == Console.LineTypeError){
            this.classes.push("error")
        } else if(line.type == Console.LineTypeDefault){
            this.classes.push("default")
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