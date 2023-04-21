import React from "react";
import ReactDOM from "react-dom";
import { createRoot } from 'react-dom/client';
import App from "./components/App";
import "./index.css";
import PacketTypes from "logic/PacketTypes";

PacketTypes();

const domNode = document.getElementById('root');
const root = createRoot(domNode);

root.render(<App />);
