import Strings from "msl/util/Strings";

class DateTime {
    static getTimeString(date){
        if(date === undefined){
            date = new Date(); 
        }
        var h = date.getHours();
        var m = date.getMinutes();
        var s = date.getSeconds();
        return Strings.pad("00", h, true) + ":" + Strings.pad("00", m, true) + ":" + Strings.pad("00", s, true);
    }
}



export default DateTime