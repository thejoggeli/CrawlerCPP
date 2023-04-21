import Log from "msl/log/Log";
import Connection from "msl/remote/Connection"
import Packet from "msl/remote/Packet";
import PacketReceiver from "msl/remote/PacketReceiver";
import PacketRecycler from "msl/remote/PacketRecycler";
import PacketSender from "msl/remote/PacketSender";
import Time from "msl/time/Time";
import Messenger from "msl/util/Messenger";
import Subscribable from "msl/util/Subscribable";

class Main {

    static messenger = new Messenger();
    static connection = new Connection({"port": 9090});
    static packetReceiver = new PacketReceiver();
    static packetSender = new PacketSender();
    static packetRecycler = new PacketRecycler();
    static events = new Subscribable();

    static init(){
        Main.connection.subscribe("Open", Main, () => {});
        Main.connection.subscribe("OpenFailed", Main, Main.onConnectionOpenFailed);
        Main.connection.subscribe("Close", Main, Main.onConnectionClose);
        Main.connection.subscribe("Error", Main, Main.onConnectionError);
        Main.connection.subscribe("Message", Main, Main.onConnectionMessage);        
        Main.connection.connect();
    }

    static update(){

        // update time
        Time.update();

        // process received packets
        Main.packetReceiver.processPackets()
    
        // process messages
        Main.messenger.processMessages()

        // notify update
        Main.events.notifySubscribers("Update")

        // send packets
        var packets = Main.packetSender.getPackets()
        for (var p in packets) {
            var packet = packets[p]
            Main.connection.send(packet.buffer.getDataView())
            Main.bytesSent += packet.buffer.getDataSize()
            Main.packetsSent += 1
            // add packet to recycler
            var type = packet.type.name
            if(Main.packetRecycler.hasGroup(type)){
                packet.reset()
                Main.packetRecycler.pushPacket(type, packet)
            } else {
                Log.warning("Main", "packet of type=" + type + " was not recycled")
            }
        }
        Main.packetSender.clearPackets()

    }

    static createPacket(type){
        return Main.packetRecycler.popPacket(type);
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
        Log.debug("Main", "onConnectionMessage");
        var packet = new Packet()
        packet.unpack(event.data)
        Main.packetReceiver.addPacket(packet)
    }

    static isConnected(){
        return Main.connection.connected;
    }

}

export default Main