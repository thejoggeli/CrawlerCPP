#include <iostream>
#include "comm/SerialStream.h"
#include "parts/XYZServo.h"

using namespace std;

int error_counter = 0;

SerialStream stream;

void read_servos_eeprom(){

    int eeprom_start = 0;
    int eeprom_length = 15;
    
    // print table header
    printf("ID ");
    for(int i = eeprom_start; i < eeprom_start+eeprom_length; i++){
        printf("%02d ", i);
    }

    printf("\n");
    // print table horizontal line
    for(int i = eeprom_start; i < eeprom_start+eeprom_length; i++){
        printf("---");
    }
    printf("\n");

    // loop through servo ids
    for(int servo_id = 0; servo_id <= 20; servo_id++){

        // create servo object
        XYZServo servo(&stream, servo_id);

        // read eeprom
        uint8_t eeprom[80];
        servo.eepromRead(eeprom_start, eeprom+eeprom_start, eeprom_length);

        // print eeprom contents
        printf("%02d ", servo_id);
        int error = servo.getLastError();
        if(error){
            // eeprom read failed
            error_counter++;
            printf("error code %d", error);
        } else {
            for(int i = eeprom_start; i < eeprom_start+eeprom_length; i++){
                printf("%02x ", eeprom[i]);
            } 
        }
        printf("\n");

    }
}

void test_servo_oszi(int servo_id){

    XYZServo servo(&stream, servo_id);

    servo.reboot();
    
    usleep(3000*1000);

    while(1){

        cout << "servo.readStatus()" << endl;
        XYZServoStatus status = servo.readStatus();

        int error = servo.getLastError();
        if(error){
            cout << "error code " << error << endl;
            error_counter++;
        } else {
            cout << "status.iBus = " << status.iBus << endl;
            cout << "status.position = " << status.position << endl;
            cout << "status.posRef = " << status.posRef << endl;
            cout << "status.pwm = " << status.pwm << endl;
            cout << "status.statusDetail = " << (int)status.statusDetail << endl;
            cout << "status.statusError = " << (int)status.statusError << endl;
        }

        int playtime_ms;

        cout << "servo.setPosition()" << endl;
        playtime_ms = 2500;
        servo.setPosition(511, playtime_ms/10);
        usleep(playtime_ms*1000+1000000);

        cout << "servo.setPosition()" << endl;
        playtime_ms = 2500;
        servo.setPosition(511+255, playtime_ms/10);
        usleep(playtime_ms*1000+1000000);

    }

}

void read_servo_status(int servo_id){

    XYZServo servo(&stream, servo_id);

    cout << "servo.readStatus()" << endl;
    XYZServoStatus status = servo.readStatus();

    int error = servo.getLastError();
    if(error){
        cout << "error code " << error << endl;
        error_counter++;
    } else {
        cout << "status.iBus = " << status.iBus << endl;
        cout << "status.position = " << status.position << endl;
        cout << "status.posRef = " << status.posRef << endl;
        cout << "status.pwm = " << status.pwm << endl;
        cout << "status.statusDetail = " << (int)status.statusDetail << endl;
        cout << "status.statusError = " << (int)status.statusError << endl;
    }

}

void move_servo(int servo_id){
    
    XYZServo servo(&stream, servo_id);

    int playtime_ms;

    cout << "servo.setPosition()" << endl;
    playtime_ms = 2000;
    servo.setPosition(511, playtime_ms/10);
    usleep(playtime_ms*1000);
    
    cout << "servo.setPosition()" << endl;
    playtime_ms = 1000;
    servo.setPosition(511-128, playtime_ms/10);
    usleep(playtime_ms*1000);
    
    cout << "servo.setPosition()" << endl;
    playtime_ms = 1000;
    servo.setPosition(511+128, playtime_ms/10);
    usleep(playtime_ms*1000);
    
    cout << "servo.setPosition()" << endl;
    playtime_ms = 250;
    servo.setPosition(511, playtime_ms/10);
    usleep(playtime_ms*1000);

}

void print_servo_position(int servo_id){
    XYZServo servo(&stream, servo_id);
    XYZServoStatus status = servo.readStatus();
    if(servo.getLastError()){
        printf("%02d status error\n", servo_id);
        error_counter++;
    } else {
        printf("%02d position %d\n", servo_id, status.position);
    }
}

void reboot_servos(){
    cout << "servo.reboot()" << endl;
    for(int i = 0; i <= 20; i++){
        XYZServo servo(&stream, i);
        servo.reboot();
    }
    usleep(2500*1000);
}


int main(){

    // open stream
    cout << "stream.open() ";
    int stream_open_ret = stream.open("/dev/ttyTHS0", 115200);
    if(!stream_open_ret){
        cout << "failed" << endl;
        return EXIT_FAILURE;
    } else {
        cout << "success" << endl;
    }

    // do stuff
    // test_servo_oszi(5);
    reboot_servos();

    read_servos_eeprom();

    uint8_t servo_ids[] = {1,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
    const unsigned int NUM_SERVOS = sizeof(servo_ids)/sizeof(uint8_t);

    int playtimes[] = {2000, 1500, 1000, 500, 400, 300, 200, 150, 100};
    const unsigned int NUM_PLAYTIMES = sizeof(playtimes)/sizeof(int);

    for(int i = 0; i < NUM_SERVOS; i++){
        XYZServo servo(&stream, servo_ids[i]);
        servo.writeAlarmLedPolicyRam(0x0F);
        servo.writeLedControl(0x06);
    }

    for(int k = 0; k < NUM_PLAYTIMES; k++){

        int playtime = playtimes[k];

        for(int i = 0; i < NUM_SERVOS; i++){
            XYZServo servo(&stream, servo_ids[i]);
            servo.setPosition(650, playtime/10);
        }
        usleep(playtime*1000); 

        for(int i = 0; i < NUM_SERVOS; i++){
            print_servo_position(servo_ids[i]);
        }

        {
            uint16_t positions[NUM_SERVOS];
            for(int i = 0; i < NUM_SERVOS; i++){
                positions[i] = 700;
            }
            XYZServo servo254(&stream, 254);
            servo254.setPositionsSync(positions, servo_ids, (uint8_t)(playtime/10), (uint8_t)NUM_SERVOS);
            usleep(playtime*1000);
        }

        for(int i = 0; i < NUM_SERVOS; i++){
            print_servo_position(servo_ids[i]);
        }

        for(int i = 0; i < NUM_SERVOS; i++){
            XYZServo servo(&stream, servo_ids[i]);
            servo.writeLedControl(k);
        }

    }

    usleep(2500*1000);

    {
        int playtime = 0;
        uint16_t positions[NUM_SERVOS];
        uint8_t playtimes[NUM_SERVOS];
        XYZServo servo254(&stream, 254);

        for(int i = 0; i < NUM_SERVOS; i++){
            positions[i] = 700 - 5*(i+1);
            playtimes[i] = (200 + i*100) / 10;
        }
        servo254.setPositions(positions, servo_ids, playtimes, NUM_SERVOS);
        usleep((playtimes[NUM_SERVOS-1]*10+10)*1000); 

        for(int i = 0; i < NUM_SERVOS; i++){
            positions[i] = 700;
            playtimes[i] = (200 + i*100) / 10;
        }
        servo254.setPositions(positions, servo_ids, playtimes, NUM_SERVOS);
        usleep((playtimes[NUM_SERVOS-1]*10+10)*1000); 
    }

    usleep(2500*1000);

    reboot_servos();

    usleep(2500*1000);

    for(int i = 0; i < NUM_SERVOS; i++){
        XYZServo servo(&stream, servo_ids[i]);
        servo.writeAlarmLedPolicyRam(0x0F);
        servo.writeLedControl(i);
    }

    for(int k = 0; k < 20; k++){
        for(int i = 0; i < NUM_SERVOS; i++){
            print_servo_position(servo_ids[i]);
        }
    }

    // for(int i = 0; i <= 20; i++){
    //     move_servo(i);
    // }
    // read_servo_status(15);
    // read_servos_eeprom();

    // close stream
    cout << "stream.close()" << endl;
    stream.close();

    cout << "error_counter: " << error_counter << endl;

    // done
    return EXIT_SUCCESS;

}