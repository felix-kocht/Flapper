#include <Arduino.h>
#include <Math.h>   
#include <Servo.h>  
#include "ServoInit.h"  
#include "utils.h"  

// parameters: Mode(false = angle, true = speed), Pin, initial angle/speed, minimum angle/speed, maximum angle/speed
ServoParams heave_servo_params = {true, 9, 500, 1500, 2500}; // heave servo (Pin 9, speed control)
ServoParams pitch_servo_right_params = {false, 8, 0, 90, 180}; // pitch servo right (Pin 8, angle control)
ServoParams pitch_servo_left_params = {false, 7, 0, 90, 180}; // pitch servo left (Pin 7, angle control)
ServoParams camber_servo_right_params = {false, 6, 0, 90, 180}; // camber servo right (Pin 6, angle control)
ServoParams camber_servo_left_params = {false, 5, 0, 90, 180}; // camber servo left (Pin 5, angle control)

// Create servo instances
Servo heave_servo;
Servo pitch_servo_right;
Servo pitch_servo_left;
Servo camber_servo_right;
Servo camber_servo_left;

//function prototypes
float setHeave(float target_pos);
float SerialRead();

//control variables
unsigned long start_time = 0;
float sine_params[6][5] = {0}; //for all 5 servos, (A,f,phase,offset,deadband(min & max))
float (*sine_params_ptr)[5] = sine_params; //pointer needed for use in funtions
float target_pos_heave = 0;
float target_pos_pitch_right = 0;
float target_pos_pitch_left = 0;
float target_pos_camber_right = 0;
float target_pos_camber_left = 0;

void setup() {
    Serial.begin(9600);  // Initialize serial communication
    Serial.println("Starting setup");
    start_time = millis();
    // Initialize servos
    initServo(heave_servo, heave_servo_params);
    initServo(pitch_servo_right, pitch_servo_right_params);
    initServo(pitch_servo_left, pitch_servo_left_params);
    initServo(camber_servo_right, camber_servo_right_params);
    initServo(camber_servo_left, camber_servo_left_params);
}

void loop() {
    //setpoints
    /* // configure to change sine movement of any servo (A,f,phase,offset,deadband(min & max), current time in ms)
    target_pos_heave = sineWave(1200, 0.8, 0, 1500, 450, 450, millis()-start_time);//0.13Hz is as fast as Heave servo can go at 600 amplitude
    target_pos_pitch_right = sineWave(40, 10*0.13, M_PI, 90, 0, 0, millis()-start_time);
    target_pos_pitch_left = sineWave(40, 10*0.13, 0, 90, 0, 0, millis()-start_time);
    target_pos_camber_right = sineWave(90, 0.13, M_PI, 90, 0, 0, millis()-start_time);
    target_pos_camber_left = sineWave(90, 0.13, 0, 90, 0, 0, millis()-start_time); */

    target_pos_heave = sineWave(sine_params_ptr[0][0], sine_params_ptr[0][1], sine_params_ptr[0][2], 
                                      sine_params_ptr[0][3], sine_params_ptr[0][4], sine_params_ptr[0][4], 
                                      millis() - start_time);

    target_pos_pitch_right = sineWave(sine_params_ptr[1][0], sine_params_ptr[1][1], sine_params_ptr[1][2], 
                                            sine_params_ptr[1][3], sine_params_ptr[1][4], sine_params_ptr[1][4], 
                                            millis() - start_time);
    target_pos_pitch_left = sineWave(sine_params_ptr[2][0], sine_params_ptr[2][1], sine_params_ptr[2][2], 
                                           sine_params_ptr[2][3], sine_params_ptr[2][4], sine_params_ptr[2][4], 
                                           millis() - start_time);

    target_pos_camber_right = sineWave(sine_params_ptr[3][0], sine_params_ptr[3][1], sine_params_ptr[3][2], 
                                             sine_params_ptr[3][3], sine_params_ptr[3][4], sine_params_ptr[3][4], 
                                             millis() - start_time);

    target_pos_camber_left = sineWave(sine_params_ptr[4][0], sine_params_ptr[4][1], sine_params_ptr[4][2], 
                                            sine_params_ptr[4][3], sine_params_ptr[4][4], sine_params_ptr[4][4], 
                                            millis() - start_time);
    //control
    //float target_speed_heave = setHeave(target_pos_heave);

    // Write target values to servos
    heave_servo.writeMicroseconds(target_pos_heave); //change back to target heave speed
    pitch_servo_right.write(target_pos_pitch_right);
    pitch_servo_left.write(target_pos_pitch_left);
    camber_servo_right.write(target_pos_camber_right);
    camber_servo_left.write(target_pos_camber_left);

    //DEBUG
    Serial.println(target_pos_heave);
}

float setHeave(float target_pos){
    float measurement = SerialRead();
    if (measurement == 0){
        return 1500;
    }
    float error = target_pos - measurement;
    float heave =  (error)+1500;
    //serially writing measurement, target position, error and heave
    Serial.println(heave);
    return heave;
}

float SerialRead(){
    if(Serial.available() > 0){
        return Serial.parseFloat();
    }else{
        return 0;
    }
}
