import Enum from "msl/util/Enum";

var JointDataType = new Enum([
    {id: 0, name: "TargetAngle"},
    {id: 1, name: "MeasuredAngle"},
    {id: 2, name: "Temperature"},
    {id: 3, name: "Voltage"},
    {id: 4, name: "Current"},
    {id: 5, name: "Weight"},
    {id: 6, name: "Distance"},
])

export default JointDataType