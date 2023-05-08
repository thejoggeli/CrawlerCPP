export default function Bitwise(){}

Bitwise.getBit = function(value, bitNumber){
    return (value>>bitNumber)&1
}

Bitwise.isBitSet = function(value, bitNumber){
    return (value>>bitNumber)&1 ? true : false
}

Bitwise.setBitTo = function(value, bitNumber, bitValue){
    if(bitValue){
        return value | (1<<bitNumber)
    }
    return value & ~(1<<bitNumber)
}

Bitwise.setBit = function(value, bitNumber){
    return value | (1<<bitNumber)
}

Bitwise.clearBit = function(value, bitNumber){
    return value & ~(1<<bitNumber)
}