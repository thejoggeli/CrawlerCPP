#include "Robot.h"
#include "Leg.h"
#include "Joint.h"
#include "brain/Brain.h"
#include "parts/XYZServo.h"
#include "comm/SerialStream.h"
#include "libi2c/i2c.h"
#include "math/Mathf.h"
#include "core/Log.h"
#include "core/Time.h"
#include "core/Config.h"
#include <string>

using namespace std;

namespace Crawler {

Robot::Robot(){ 

}

Robot::~Robot(){ 
    for(Leg* leg : legs){
        delete leg;
    }
    legs.clear();
    delete masterServo;
}

void Robot::Init(){

    // all servo ids in order [hip, knee, knee, knee] 
    servoIds = {
        11, 12, 13, 14, // front-left  
        21, 22, 23, 24, // back-left   
        31, 32, 33, 34, // back-right  
        41, 42, 43, 44, // front right 
    };

    // weight sensor data pins
    unsigned int weightSensorDataPins[] = {
        4, 4, 4, 4
    };

    // weight sensor clock pins
    unsigned int weightSensorClockPins[] = {
        17, 17, 17, 17
    };

    // leg names
    string legNames[4] = {"FL", "BL", "BR", "FR"};

    // create master servo
    masterServo = new XYZServo(servoSerialStream, 254);

    // create joint servos
    for(int i = 0; i < servoIds.size(); i++){
        XYZServo* servo = new XYZServo(servoSerialStream, servoIds[i]);
        jointServos.push_back(servo);
    }

    // create the legs
    for(int i = 0; i < 4; i++){

        Leg* leg = new Leg(this, i, legNames[i]);
        legs.push_back(leg);

        // init distance sensor
        if(!leg->InitDistanceSensor()){
            LogError("Robot", iLog << "InitDistanceSensor() failed, legId=" << leg->id);
        } else {
            LogError("Robot", iLog << "InitDistanceSensor() success, legId=" << leg->id);
        }

        // init weight sensor
        if(!leg->InitWeightSensor(weightSensorDataPins[i], weightSensorClockPins[i])){
            LogError("Robot", iLog << "InitWeightSensor() failed, legId=" << leg->id);
        } else {
            LogError("Robot", iLog << "InitWeightSensor() success, legId=" << leg->id);
        }

        // add all joints of current leg to jointsList
        for(Joint* joint : leg->joints){
            jointsList.push_back(joint);
        }

    }

    // assign servo motors to all joints
    for(int i = 0; i < jointsList.size(); i++){
        jointsList[i]->SetServo(jointServos[i]);
    }

    // calibrate servos
    legs[0]->joints[0]->SetCalibrationAnglesLinear(-90.0f*DEG_2_RADf, 0.0f);
    legs[1]->joints[0]->SetCalibrationAnglesLinear(0.0f, +90.0f*DEG_2_RADf);
    legs[2]->joints[0]->SetCalibrationAnglesLinear(-90.0f*DEG_2_RADf, 0.0f);
    legs[3]->joints[0]->SetCalibrationAnglesLinear(0.0f, +90.0f*DEG_2_RADf);
    for(int i = 0; i < 4; i++){
        legs[i]->joints[1]->SetCalibrationAnglesQuadratic(-90.0f*DEG_2_RADf, 0.0f*DEG_2_RADf, +90.0f*DEG_2_RADf);
        legs[i]->joints[2]->SetCalibrationAnglesQuadratic(-90.0f*DEG_2_RADf, 0.0f*DEG_2_RADf, +90.0f*DEG_2_RADf);
        legs[i]->joints[3]->SetCalibrationAnglesQuadratic(-90.0f*DEG_2_RADf, 0.0f*DEG_2_RADf, +90.0f*DEG_2_RADf);
    }
    for(unsigned int i = 0; i < 16; i++){
        unsigned int calib_values[3] = {
            (unsigned int) Config::GetInt("calib", "low_"+to_string(i), 256),
            (unsigned int) Config::GetInt("calib", "mid_"+to_string(i), 512),
            (unsigned int) Config::GetInt("calib", "high_"+to_string(i), 768),
        };        
        jointsList[i]->SetCalibrationValues(calib_values);
        jointsList[i]->UpdateCalibrationFactors();
    }

    // set legs hip transform
    float hip_dx = 0.065f;
    float hip_dy = 0.065f;
    float hip_dz = 0.0f;   
    legs[0]->SetHipTransform(Eigen::Vector3f(+hip_dx, +hip_dy, hip_dz), 90.0f * DEG_2_RADf);
    legs[1]->SetHipTransform(Eigen::Vector3f(-hip_dx, +hip_dy, hip_dz), 90.0f * DEG_2_RADf);
    legs[2]->SetHipTransform(Eigen::Vector3f(-hip_dx, -hip_dy, hip_dz), -90.0f * DEG_2_RADf);
    legs[3]->SetHipTransform(Eigen::Vector3f(+hip_dx, -hip_dy, hip_dz), -90.0f * DEG_2_RADf);

    // set hip joints angle limits
    legs[0]->joints[0]->limitMin = -90.0f * DEG_2_RADf;
    legs[0]->joints[0]->limitMax =   0.0f * DEG_2_RADf;
    legs[1]->joints[0]->limitMin =   0.0f * DEG_2_RADf;
    legs[1]->joints[0]->limitMax =  90.0f * DEG_2_RADf;
    legs[2]->joints[0]->limitMin = -90.0f * DEG_2_RADf;
    legs[2]->joints[0]->limitMax =   0.0f * DEG_2_RADf;
    legs[3]->joints[0]->limitMin =   0.0f * DEG_2_RADf;
    legs[3]->joints[0]->limitMax =  90.0f * DEG_2_RADf;

    for(int i = 0; i < 4; i++){

        // set knee joints angle limits
        legs[i]->joints[1]->limitMin = -100.0f * DEG_2_RADf;
        legs[i]->joints[1]->limitMax = +100.0f * DEG_2_RADf;
        legs[i]->joints[2]->limitMin = -150.0f * DEG_2_RADf;
        legs[i]->joints[2]->limitMax = +150.0f * DEG_2_RADf;
        legs[i]->joints[3]->limitMin = -90.0f * DEG_2_RADf;
        legs[i]->joints[3]->limitMax = +90.0f * DEG_2_RADf;

        // set joints lengths
        legs[i]->joints[0]->length = 0.058;
        legs[i]->joints[1]->length = 0.078;
        legs[i]->joints[2]->length = 0.078;
        legs[i]->joints[3]->length = 0.078;
    }
}

void Robot::SetBrain(Brain* brain){

    if(this->brain != nullptr){
        this->brain->Destroy();
        delete this->brain;
    }

    this->brain = brain;
    this->brain->SetRobot(this);

    brain->Init();

}

bool Robot::OpenSerialStream(const char* device){
    if(servoSerialStream){
        LogError("Robot", "servoSerialStream is not null");
        return false;
    }
    // create serial stream for servo communication
    servoSerialStream = new SerialStream();
    int ret = servoSerialStream->open(device, 115200);
    return ret;
}

void Robot::CloseSerialStream(){
    servoSerialStream->close();
}

bool Robot::OpenI2CDevice(const char* device){
    if(i2cDevice){
        LogError("Robot", "i2cDevice is not null");
        return false;
    }
    // create i2c device
    int bus = i2c_open(device);
    if(bus == -1){
        LogError("Robot", iLog << "i2c_open() failed, bus=" << (int)bus);
        return false;
    }
    i2cDevice = new I2CDevice();
    i2cDevice->bus = bus;
    i2cDevice->addr = 0x13;
    i2cDevice->iaddr_bytes = 1;
    i2cDevice->page_bytes = 16;
    i2cDevice->tenbit = false;
    return true;
}

void Robot::CloseI2CDevice(){
    if(!i2cDevice){
        return;
    }
    i2c_close(i2cDevice->bus);
}

void Robot::MoveJointsToTargetSync(float time, bool forceTorqueOn){
    static uint8_t ids[16];
    static uint16_t positions[16];
    int size = 0;
    for(int i = 0; i < jointsList.size(); i++){
        Joint* joint = jointsList[i];
        if(joint->servoState == ServoState::OK || (joint->servoState == ServoState::TorqueOff && forceTorqueOn)){
            ids[size] = joint->servo->id;
            positions[size] = joint->AngleToXYZ(joint->currentTargetAngle);
            size++;
        }
    }
    uint8_t playtime = (uint8_t)(time*100.0f);
    masterServo->setPositionsSync(positions, ids, playtime, size);
}

void Robot::Update(){
    brain->Update();
}

void Robot::FixedUpdate(){
    brain->FixedUpdate();
}

void Robot::RebootServos(float sleepTime){
    LogInfo("Robot", iLog << "RebootServos(sleepTime=" << sleepTime << ")");
    for(Joint* joint : jointsList){
        joint->RebootServo();
    }
    Time::Sleep(sleepTime);
}

bool Robot::PingServos(){
    LogInfo("Robot", "PingServos()");
    bool result = true;
    for(Joint* joint : jointsList){
        joint->PingServo();
        if(!joint->lastPingServoResult){
            result = false;
        }
    }
    if(result){
        LogInfo("Robot", "PingServos() success");
    } else {
        LogError("Robot", "PingServos() failed");
    }
    return result;
}

void Robot::Startup(){

    LogInfo("Robot", "Startup()");

    // set LED colors
    SetServosLedPolicyUser();
    for(Joint* joint : jointsList){
        joint->SetServoLedColor(1, 0, 1, 0);
    }

    // move servos to initial position
    LogInfo("Robot", "moving to default position");
    for(Leg* leg : legs){
        // leg->joints[1]->SetTargetAngle(DEG_2_RADf * +30.0f);
        // leg->joints[2]->SetTargetAngle(DEG_2_RADf * +30.0f);
        // leg->joints[3]->SetTargetAngle(DEG_2_RADf * +30.0f);
        leg->joints[1]->SetTargetAngle(DEG_2_RADf * -60.0f);
        leg->joints[2]->SetTargetAngle(DEG_2_RADf * +120.0f);
        leg->joints[3]->SetTargetAngle(DEG_2_RADf * +30.0f);
    }
    // legs[0]->joints[0]->SetTargetAngle(DEG_2_RADf * -45.0f);
    // legs[1]->joints[0]->SetTargetAngle(DEG_2_RADf * +45.0f);
    // legs[2]->joints[0]->SetTargetAngle(DEG_2_RADf * -45.0f);
    // legs[3]->joints[0]->SetTargetAngle(DEG_2_RADf * +45.0f);
    legs[0]->joints[0]->SetTargetAngle(DEG_2_RADf * -25.0f);
    legs[1]->joints[0]->SetTargetAngle(DEG_2_RADf * +25.0f);
    legs[2]->joints[0]->SetTargetAngle(DEG_2_RADf * -25.0f);
    legs[3]->joints[0]->SetTargetAngle(DEG_2_RADf * +25.0f);
    MoveJointsToTargetSync(2.0f);
    Time::Sleep(2.5f);

    // set LED colors
    for(Leg* leg : legs){
        leg->joints[0]->SetServoLedColor(0, 0, 1, 0);
        leg->joints[1]->SetServoLedColor(0, 0, 1, 0);
        leg->joints[2]->SetServoLedColor(0, 0, 1, 0);
        leg->joints[3]->SetServoLedColor(0, 0, 1, 0);
    }

    // initialize joint states
    for(Joint* joint : jointsList){
        joint->ReadMeasuredStatus();
        joint->lastTargetAngle = joint->currentTargetAngle;
    }
    
}

void Robot::SetServosLedPolicyUser(bool buffer){
    for(Joint* joint : jointsList){
        joint->SetServoLedPolicyUser(buffer);
    }
}
void Robot::SetServosLedPolicySystem(bool buffer){
    for(Joint* joint : jointsList){
        joint->SetServoLedPolicySystem(buffer);
    }
}

void Robot::Shutdown(){

    LogInfo("Robot", "Shutdown()");

    for(Joint* joint : jointsList){
        joint->TorqueOff();
    }

}

void Robot::PrintServoStatus(){
    LogInfo("Robot", "Status Error:");
    char buffer[100];
    for(Leg* leg : legs){
        sprintf(buffer, "%s %02X %02X %02X %02X", leg->name.c_str(),
            leg->joints[0]->statusError.value,
            leg->joints[1]->statusError.value,
            leg->joints[2]->statusError.value,
            leg->joints[3]->statusError.value
        );
        LogInfo("Robot", buffer);
    }
    LogInfo("Robot", "Status Detail:");
    for(Leg* leg : legs){
        sprintf(buffer, "%s %02X %02X %02X %02X", leg->name.c_str(),
            leg->joints[0]->statusDetail.value,
            leg->joints[1]->statusDetail.value,
            leg->joints[2]->statusDetail.value,
            leg->joints[3]->statusDetail.value
        );
        LogInfo("Robot", buffer);
    }
    LogInfo("Robot", "Target Angles (deg):");
    for(Leg* leg : legs){
        sprintf(buffer, "%s %6.1f %6.1f %6.1f %6.1f", leg->name.c_str(),
            leg->joints[0]->currentTargetAngle * RAD_2_DEGf,
            leg->joints[1]->currentTargetAngle * RAD_2_DEGf,
            leg->joints[2]->currentTargetAngle * RAD_2_DEGf,
            leg->joints[3]->currentTargetAngle * RAD_2_DEGf
        );
        LogInfo("Robot", buffer);
    }
    LogInfo("Robot", "Measured Angles (deg):");
    for(Leg* leg : legs){
        sprintf(buffer, "%s %6.1f %6.1f %6.1f %6.1f", leg->name.c_str(),
            leg->joints[0]->measuredAngle.value * RAD_2_DEGf,
            leg->joints[1]->measuredAngle.value * RAD_2_DEGf,
            leg->joints[2]->measuredAngle.value * RAD_2_DEGf,
            leg->joints[3]->measuredAngle.value * RAD_2_DEGf
        );
        LogInfo("Robot", buffer);
    }
    LogInfo("Robot", "(Measured - Target) Angles (deg):");
    for(Leg* leg : legs){
        sprintf(buffer, "%s %6.1f %6.1f %6.1f %6.1f", leg->name.c_str(),
            (leg->joints[0]->measuredAngle.value - leg->joints[0]->currentTargetAngle) * RAD_2_DEGf,
            (leg->joints[1]->measuredAngle.value - leg->joints[1]->currentTargetAngle) * RAD_2_DEGf,
            (leg->joints[2]->measuredAngle.value - leg->joints[2]->currentTargetAngle) * RAD_2_DEGf,
            (leg->joints[3]->measuredAngle.value - leg->joints[3]->currentTargetAngle) * RAD_2_DEGf
        );
        LogInfo("Robot", buffer);
    }
    LogInfo("Robot", "Measured Temperature (degC):");
    for(Leg* leg : legs){
        sprintf(buffer, "%s %6.1f %6.1f %6.1f %6.1f", leg->name.c_str(),
            leg->joints[0]->measuredTemperature.value,
            leg->joints[1]->measuredTemperature.value,
            leg->joints[2]->measuredTemperature.value,
            leg->joints[3]->measuredTemperature.value
        );
        LogInfo("Robot", buffer);
    }
    LogInfo("Robot", "Measured Voltage (V):");
    for(Leg* leg : legs){
        sprintf(buffer, "%s %6.1f %6.1f %6.1f %6.1f", leg->name.c_str(),
            leg->joints[0]->measuredVoltage.value,
            leg->joints[1]->measuredVoltage.value,
            leg->joints[2]->measuredVoltage.value,
            leg->joints[3]->measuredVoltage.value
        );
        LogInfo("Robot", buffer);
    }
    float currentSumRobot = 0;
    for(Joint* joint : jointsList){
        currentSumRobot += joint->measuredCurrent.value;
    }
    LogInfo("Robot", "Measured Current (mA):");
    for(Leg* leg : legs){
        float currentSumLeg = 0;
        for(Joint* joint : leg->joints){
            currentSumLeg += joint->measuredCurrent.value;
        }
        sprintf(buffer, "%s %6.1f %6.1f %6.1f %6.1f   sum = %6.1f", leg->name.c_str(),
            leg->joints[0]->measuredCurrent.value,
            leg->joints[1]->measuredCurrent.value,
            leg->joints[2]->measuredCurrent.value,
            leg->joints[3]->measuredCurrent.value,
            currentSumLeg
        );
        LogInfo("Robot", buffer);
    }
    LogInfo("Robot", iLog << "Total Current (mA): " << currentSumRobot);
}

void Robot::TorqueOn(bool buffer){
    for(Leg* leg : legs){
        leg->TorqueOn(buffer);
    }
}

void Robot::TorqueOff(bool buffer){
    for(Leg* leg : legs){
        leg->TorqueOff(buffer);
    }
}

}