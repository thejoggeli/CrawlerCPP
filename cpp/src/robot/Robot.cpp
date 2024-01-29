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
#include "parts/MuxI2C.h"
#include "comm/I2CBus.h"
#include "parts/IMU.h"

using namespace std;

namespace Crawler {

Robot::Robot(){ 

}

Robot::~Robot(){ 

    // delete legs
    LogInfo("Robot", "deleting legs");
    for(Leg* leg : legs){
        delete leg;
    }

    // delete servos
    LogInfo("Robot", "deleting servos");
    for(XYZServo* servo : jointServos){
        delete servo;
    }
    if(masterServo){
        delete masterServo;
    }

    // delete imu
    LogInfo("Robot", "deleting IMU");
    if(imu){
        delete imu;
    }

    // delete muxers
    LogInfo("Robot", "deleting mux0");
    if(mux0){
        mux0->Shutdown();
        delete mux0;
    }
    LogInfo("Robot", "deleting mux1");
    if(mux1){
        mux1->Shutdown();
        delete mux1;
    }

    // closes i2c bus
    LogInfo("Robot", "deleting i2c bus0");
    if(bus0){
        bus0->Close();
        delete bus0;
    }
    LogInfo("Robot", "deleting i2c bus1");
    if(bus1){
        bus1->Close();
        delete bus1;
    }

    // close serial stream
    LogInfo("Robot", "deleting servo serial stream");
    if(servoSerialStream){
        servoSerialStream->close();
        delete servoSerialStream;
    }
}

bool Robot::Init(){

    // imu
    imu = new IMU();
    if(!imu->Init(bus1->fd)){
        LogError("Robot", iLog << "IMU init failed");
        // return false;
    }

    // all servo ids in order [hip, knee, knee, knee] 
    servoIds = {
        11, 12, 13, 14, // front-left  
        21, 22, 23, 24, // back-left   
        31, 32, 33, 34, // back-right  
        41, 42, 43, 44, // front right 
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
        if(!leg->InitDistanceSensor(mux1, i)){
            LogError("Robot", iLog << "InitDistanceSensor() failed, legId=" << leg->id);
            return false;
        } else {
            LogInfo("Robot", iLog << "InitDistanceSensor() success, legId=" << leg->id);
        }

        // init weight sensor
        if(!leg->InitWeightSensor(mux0, i)){
            LogError("Robot", iLog << "InitWeightSensor() failed, legId=" << leg->id);
            return false;
        } else {
            LogInfo("Robot", iLog << "InitWeightSensor() success, legId=" << leg->id);
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
    for(int i = 0; i < 4; i++){
        legs[i]->joints[0]->SetCalibrationAngles(-45.0f*DEG_2_RADf, 45.0f*DEG_2_RADf);
        legs[i]->joints[1]->SetCalibrationAngles(-90.0f*DEG_2_RADf, 0.0f*DEG_2_RADf, +90.0f*DEG_2_RADf);
        legs[i]->joints[2]->SetCalibrationAngles(-90.0f*DEG_2_RADf, 0.0f*DEG_2_RADf, +90.0f*DEG_2_RADf);
        legs[i]->joints[3]->SetCalibrationAngles(0.0f*DEG_2_RADf, +90.0f*DEG_2_RADf);
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
    legs[0]->SetHipTransform(Eigen::Vector3f(+hip_dx, +hip_dy, hip_dz), 45.0f * DEG_2_RADf);
    legs[1]->SetHipTransform(Eigen::Vector3f(-hip_dx, +hip_dy, hip_dz), 135.0f * DEG_2_RADf);
    legs[2]->SetHipTransform(Eigen::Vector3f(-hip_dx, -hip_dy, hip_dz), -135.0f * DEG_2_RADf);
    legs[3]->SetHipTransform(Eigen::Vector3f(+hip_dx, -hip_dy, hip_dz), -45.0f * DEG_2_RADf);

    for(int i = 0; i < 4; i++){

        // set knee joints angle limits
        legs[i]->joints[0]->limitMin = -45.0f * DEG_2_RADf;
        legs[i]->joints[0]->limitMax = +45.0f * DEG_2_RADf;
        legs[i]->joints[1]->limitMin = -90.0f * DEG_2_RADf;
        legs[i]->joints[1]->limitMax = +90.0f * DEG_2_RADf;
        legs[i]->joints[2]->limitMin = -90.0f * DEG_2_RADf;
        legs[i]->joints[2]->limitMax = +150.0f * DEG_2_RADf;
        legs[i]->joints[3]->limitMin = -30.0f * DEG_2_RADf;
        legs[i]->joints[3]->limitMax = +90.0f * DEG_2_RADf;

        // set joints lengths
        legs[i]->joints[0]->length = 0.058;
        legs[i]->joints[1]->length = 0.078;
        legs[i]->joints[2]->length = 0.078;
        legs[i]->joints[3]->length = 0.078;
    }

    return true;

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

bool Robot::OpenI2C(){

    // open bus 0
    bus0 = new I2CBus();
    if(!bus0->Open("/dev/i2c-1")){
        LogError("Robot", iLog << "i2cBus0 Open() failed");
        return false;
    }

    // open bus 1
    bus1 = new I2CBus();
    if(!bus1->Open("/dev/i2c-8")){
        LogError("Robot", iLog << "i2cBus1 Open() failed");
        return false;
    }

    // create i2c multiplexers
    mux0 = new MuxI2C();
    mux1 = new MuxI2C();

    if(!mux0->Init(bus0->fd)){
        LogError("Robot", iLog << "mux0 init failed");
        return false;
    }

    if(!mux1->Init(bus1->fd)){
        LogError("Robot", iLog << "mux1 init failed");
        return false;
    }

    return true;

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
        leg->joints[0]->SetTargetAngle(DEG_2_RADf * 0.0f);
        leg->joints[1]->SetTargetAngle(DEG_2_RADf * -60.0f);
        leg->joints[2]->SetTargetAngle(DEG_2_RADf * +120.0f);
        leg->joints[3]->SetTargetAngle(DEG_2_RADf * +30.0f);
    }
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

void Robot::ReadIMU(){
    // read acceleration
    float acc[3];
    imu->ReadAcceleration(acc);
    imu_acc[0] = -acc[1];
    imu_acc[1] = +acc[0];
    imu_acc[2] = +acc[2];

    // read gyro
    // float gyro[3];
    // imu->ReadGyro(gyro);
    // imu_gyro[0] = -gyro[1];
    // imu_gyro[1] = +gyro[0];
    // imu_gyro[2] = +gyro[2];
}

void Robot::ReadMeasuredWeight(){
    // read acceleration
    for(Leg* leg : legs){
        leg->ReadMeasuredWeight(false);
    }
}

}