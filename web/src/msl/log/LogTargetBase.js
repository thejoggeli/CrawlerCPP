class LogTargetBase {
    constructor(){
    }
    print(level, source, ...message){
        var prefix = "[" + level.short + "][" + source
        prefix = prefix.padEnd(this.prefix_pad_size-1, this.prefix_pad_char) + "]"
        console.log(prefix, ...message)
    }
}

export default LogTargetBase
