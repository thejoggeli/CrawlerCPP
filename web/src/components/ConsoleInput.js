
import React from "react";
import "./ConsoleInput.css";
import Main from "logic/Main";
import Console from "logic/Console";

export default class ConsoleInput extends React.Component {

    constructor(){
        super()
        this.state = {
            text: "",
        }
        this.historyPtr = -1
        this.textInput = React.createRef()
        this.submitButton = React.createRef()
        this.cursorNeedsUpdate = false
    }

    componentDidMount(){
        Main.events.subscribe("update", this, this.update)
        this.textInput.current.focus()
    }

    componentWillUnmount(){
        Main.events.unsubscribe("update", this)
    }

    update(){
        if(this.cursorNeedsUpdate){
            this.cursorNeedsUpdate = false
            var el = this.textInput.current
            el.selectionStart = el.selectionEnd = el.value.length;
        }
    }

    onInputSubmit = (e) => {
        e.preventDefault();
        Console.submitText(this.state.text)
        this.historyPtr = -1
        this.setState({
            text: "",
        })
    }

    onTextChange = (e) => {
        this.setState({
            text: e.target.value,
        })
    }

    onKeyDown = (e) => {
        var historyPtrChanged = false
        if(e.keyCode == 38){
            // prev line
            if(this.historyPtr == -1){
                this.historyPtr = Console.lines.length-1
            } else {
                this.historyPtr -= 1
                if(this.historyPtr < 0){
                    this.historyPtr = Console.lines.length-1
                }
            }
            historyPtrChanged = true
        } else if(e.keyCode == 40){
            // next lne
            if(this.historyPtr == -1){
                this.historyPtr = 0
            } else {
                this.historyPtr += 1
                if(this.historyPtr >= Console.lines.length){
                    this.historyPtr = 0
                }
            }
            historyPtrChanged = true
        }
        if(historyPtrChanged){
            this.setState({
                text: Console.lines[this.historyPtr].text
            })
            this.cursorNeedsUpdate = true
        }
    }

    render(){
        return (
            <div className="console-input">
                <form onSubmit={this.onInputSubmit} >
                    <input type="text" ref={this.textInput} value={this.state.text} onChange={this.onTextChange} onKeyDown={this.onKeyDown} />
                    <input type="submit" ref={this.submitButton} value="Send" />
                </form>
            </div>
        )
    }

}