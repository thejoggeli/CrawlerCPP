import Gfw from "msl/gfw/Gfw";
import Log from "msl/log/Log";
import Connection from "msl/remote/Connection"
import Packet from "msl/remote/Packet";
import PacketReceiver from "msl/remote/PacketReceiver";
import PacketSender from "msl/remote/PacketSender";
import Time from "msl/time/Time";
import Messenger from "msl/util/Messenger";
import Subscribable from "msl/util/Subscribable";
import PacketMessage from "./PacketMessage";

class Main {

    static messenger = new Messenger();
    static connection = new Connection({"port": 9090});
    static packetReceiver = new PacketReceiver();
    static packetSender = new PacketSender();
    static events = new Subscribable();
    static packetMessages = new Subscribable();

    static init(){
        Gfw.install();
        Gfw.events.subscribe("beforeUpdate", Main, Main.update)
        Gfw.events.subscribe("resize", Main, Main.resize)
        Main.packetReceiver.subscribe("Message", Main, Main.onPacketMessage)
        Main.packetMessages.subscribe("log", Main, Main.onPacketMessageLog)
        Main.connection.subscribe("open", Main, Main.onConnectionOpen)
        Main.connection.subscribe("openFailed", Main, Main.onConnectionOpenFailed);
        Main.connection.subscribe("close", Main, Main.onConnectionClose);
        Main.connection.subscribe("error", Main, Main.onConnectionError);
        Main.connection.subscribe("message", Main, Main.onConnectionMessage);        
        Main.connection.connect();
    }

    static start(){
        Gfw.start();
    }

    static resize(){
        Main.events.notifySubscribers("resize")
    }

    static update(){

        if(Main.isConnected()){

            // process received packets
            Main.packetReceiver.processPackets()
        
            // process messages
            Main.messenger.processMessages()

            // notify update
            Main.events.notifySubscribers("update")

            // send packets
            var packets = Main.packetSender.getPackets()
            for (var p in packets) {
                var packet = packets[p]
                Main.connection.send(packet.buffer.getDataView())
                Main.bytesSent += packet.buffer.getDataSize()
                Main.packetsSent += 1
            }
            Main.packetSender.clearPackets()

        }

    }

    static onPacketMessageLog(message){
        Log.info("Main", message.message, message.params.type, message.params.msg)
    }

    static onPacketMessage(packet){
        var packetMessage = new PacketMessage(packet)
        Log.debug("Main", packetMessage.message, packetMessage)
        Main.packetMessages.notifySubscribers(packetMessage.message, packetMessage)
    }

    static addPacket(type, data){
        var packet = new Packet(type, data)
        packet.pack()
        Main.packetSender.addPacket(packet)
        return packet
    }

    static addPacketMessage(packetMessage){
        var packet = new Packet("Message", {
            "message": packetMessage.message,
            "params": packetMessage.params,
        })
        packet.pack()
        Main.packetSender.addPacket(packet)
        return packet
    }

    static onConnectionOpen(event){
        Log.debug("Main", "onConnectionOpen");
        Main.messenger.sendMessage("Connect");
        Main.events.notifySubscribers("Connect");        
    }

    static onConnectionOpenFailed(event){
        Log.debug("Main", "onConnectionOpenFailed");
        setTimeout(() => {
            Main.connection.connect();
        }, 250);
    }

    static onConnectionClose(event){
        Log.debug("Main", "onConnectionClose");
        Main.events.notifySubscribers("Disconnect");
        Main.messenger.sendMessage("Disconnect");
        setTimeout(() => {
            Main.connection.connect();
        }, 250);
    }

    static onConnectionError(event){
        Log.debug("Main", "onConnectionError");
    }

    static onConnectionMessage(event){
        // Log.debug("Main", "onConnectionMessage");
        var packet = new Packet()
        packet.unpack(event.data)
        Main.packetReceiver.addPacket(packet)
    }

    static isConnected(){
        return Main.connection.connected;
    }

}

export default Main