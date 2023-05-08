
import React from "react";
import "./ConsoleComponent.css";
import Main from "logic/Main";
import ConsoleViewport from "./ConsoleViewport";
import ConsoleInput from "./ConsoleInput";

export default class ConsoleComponent extends React.Component {

    componentDidMount(){

    }

    componentWillUnmount(){

    }

    render(){
        return (
            <div className="console">
                <ConsoleViewport />
                <ConsoleInput />
            </div>
        )
    }

}