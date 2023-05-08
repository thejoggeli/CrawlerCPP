import Strings from "msl/util/Strings.js"

class Time {
    
    static currentTime = 0.0;
    static lastTime = 0.0;
    static startTime = 0.0;
    static deltaTime = 0.0;
    
    static currentTimeMillis = 0;
    static lastTimeMillis = 0;
    static startTimeMillis = 0;
    static deltaTimeMillis = 0;

    static start(){
        Time.startTimeMillis = Time.getSystemTimeMillis();
        Time.startTime = Time.currentTimeMillis * 0.001;
    }  

    static update(){
        Time.lastTimeMillis = Time.currentTimeMillis;
        Time.currentTimeMillis = Time.getTimeMillis();
        Time.deltaTimeMillis = Time.currentTimeMillis - Time.lastTimeMillis;

        Time.lastTime = Time.lastTimeMillis * 0.001;
        Time.currentTime = Time.currentTimeMillis * 0.001
        Time.deltaTime = Time.deltaTimeMillis * 0.001;
    }

    static getTime(){
        return (Time.getSystemTimeMillis() - Time.startTimeMillis) * 0.001;
    }

    static getTimeMillis(){
        return (Time.getSystemTimeMillis() - Time.startTimeMillis);
    }

    static getSystemTimeMillis(){
        return (new Date()).getTime();
    }

    static toTimestampString(time){
        var str = ""
        var hours = Math.floor(time % (3600*24) / 3600)
        str += Strings.pad("00", hours.toString(), true)
        str += ":"
        var minutes = Math.floor((time % 3600) / 60)
        str += Strings.pad("00", minutes.toString(), true)
        str += ":"
        var seconds = Math.floor(time % 60)
        str += Strings.pad("00", seconds.toString(), true)
        return str        
    }

    static toBeautifulString(time, ms, s, m, h, d){    
        var str = ""
        if(d){
            var hours = Math.floor(time / (3600*24))
            str += Strings.pad("00", hours.toString(), true)
            if(h){
                str += ":"
            }
        }    
        if(h){
            var hours = d ? Math.floor(time % (3600*24) / 3600) : Math.floor(time / 3600)
            str += Strings.pad("00", hours.toString(), true)
            if(m){
                str += ":"
            }
        }
        if(m){
            var minutes = h ? Math.floor((time % 3600) / 60) : Math.floor(time / 60)
            str += Strings.pad("00", minutes.toString(), true)
            if(s){
                str += ":"
            }
        }
        if(s){
            var seconds = m ? Math.floor(time % 60) : Math.floor(time)
            str += Strings.pad("00", seconds.toString(), true)
            if(ms){
                str += "."
            }
        }
        if(ms){
            var millis = s ? Math.floor((time % 1.0) * 1000.0) : Math.floor(time * 1000.0)
            str += Strings.pad("000", millis.toString(), true)
        }
        return str
    }

}

export default Time