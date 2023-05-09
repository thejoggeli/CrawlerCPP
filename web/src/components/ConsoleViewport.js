
import React, { useEffect } from "react";
import "./ConsoleViewport.css";
import Main from "logic/Main";
import ConsoleLine from "./ConsoleLine";
import Time from "msl/time/Time";
import Console from "logic/Console";
import Arrays from "msl/util/Arrays";

export default class ConsoleViewport extends React.Component {

    constructor(){
        super()
        this.state = {
            linesById: {},
            lineElements: [],
        }
        this.viewport = React.createRef()
        this.newLines = 0
    }

    componentDidMount(){
        Console.events.subscribe("addLine", this, this.onAddLine)
        Console.events.subscribe("removeLine", this, this.onRemoveLine)
        Console.events.subscribe("clear", this, this.onClear)
        Main.events.subscribe("update", this, this.update)
        for(var line of Console.lines){
            this.onAddLine(line)
        }
    }

    componentWillUnmount(){
        Console.events.unsubscribe("addLine", this)
        Console.events.unsubscribe("removeLine", this)
        Console.events.unsubscribe("clear", this)
        Main.events.unsubscribe("update", this)
    }

    onAddLine(line){
        this.state.linesById[line.id] = line
        this.state.lineElements.push(<ConsoleLine key={line.id} line={line} />)
        this.setState(this.state)
        this.newLines += 1
    }

    onRemoveLine(line){
        delete this.state.linesById[line.id]
        for(var i in this.state.lineElements){
            if(this.state.lineElements[i].key == line.id){
                Arrays.removeIndex(this.state.lineElements, i)
                break;
            }
        }
        this.setState(this.state)
    }

    onClear(){
        this.state.lineElements = []
        this.state.linesById = {}
        this.setState(this.state)
    }

    update(){   
        if(this.newLines > 0){
            this.viewport.current.scrollTop = this.viewport.current.scrollHeight
            this.newLines -= 1
        }
    }

    render(){
        return (
            <div className="console-viewport" ref={this.viewport}>
                {this.state.lineElements}
            </div>
        )
    }

}