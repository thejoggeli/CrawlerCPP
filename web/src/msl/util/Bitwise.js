export default function Bitwise(){}


Bitwise.getBit = function(value, bit){
    return (value<<bit)&1
}