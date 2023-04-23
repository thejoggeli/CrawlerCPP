import React from "react";
import ReactDOM from "react-dom";
import { createRoot } from 'react-dom/client';
import App from "./components/App";
import "./index.css";
import PacketTypes from "logic/PacketTypes";
import Main from "logic/Main";
import Time from "msl/time/Time";
import Log from "msl/log/Log";
import LogTargetConsole from "msl/log/LogTargetConsole";


var logTarget = new LogTargetConsole()
Log.addTarget(logTarget)

PacketTypes();

const domNode = document.getElementById('root');
const root = createRoot(domNode);
root.render(<App />);



Main.init();
Main.start();
Time.start();