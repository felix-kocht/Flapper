#include <Arduino.h>
#include <Math.h>   
#include <Servo.h>  
#include "ServoInit.h"  
#include "setpoints.h"  

// parameters: Mode(false = angle, true = speed), Pin, initial angle/speed, minimum angle/speed, maximum angle/speed
ServoParams heave_servo_params = {true, 9, 500, 1500, 2500}; // heave servo (Pin 9, speed control)
ServoParams pitch_servo_right_params = {false, 8, 0, 90, 180}; // pitch servo right (Pin 8, angle control)
ServoParams pitch_servo_left_params = {false, 7, 0, 90, 180}; // pitch servo left (Pin 7, angle control)
ServoParams camber_servo_right_params = {false, 6, 0, 90, 180}; // camber servo right (Pin 6, angle control)
ServoParams camber_servo_left_params = {false, 5, 0, 90, 180}; // camber servo left (Pin 5, angle control)

//DEBUG
const int encoder_pin = 4; //encoder pin

// Create servo instances
Servo heave_servo;
Servo pitch_servo_right;
Servo pitch_servo_left;
Servo camber_servo_right;
Servo camber_servo_left;

//function prototypes
float setHeave(float target_pos);
float SerialRead();
float angletoPWM(float angle);

//control variables
unsigned long start_time = 0;
float Kp = 1.0; //proportional gain for controller, maybe a vector makes sense to have different gains for different servos
float control[5] = {0}; //control output for all 5 servos
float sine_params[6][5] = {0}; //for all 5 servos, operational parameters for sine wave generation (A,f,phase,offset,deadband(min & max))
float (*sine_params_ptr)[5] = sine_params; //pointer needed for use in funtions
float estimates[5] = {0}; //for all 5 servos, estimated angle
float (*estimates_ptr) = estimates; //pointer needed for use in funtions
float targets[5] = {0}; //for all 5 servos, target angle
float (*targets_ptr) = targets; //pointer needed for use in funtions
bool encoder_readings[2] = {0}; //for all 2 encoders, on or off

void setup() {
    Serial.begin(9600);  // Initialize serial communication
    Serial.println("Starting setup");
    start_time = millis();
    pinMode(encoder_pin, INPUT);
    // Initialize servos
    initServo(heave_servo, heave_servo_params);
    initServo(pitch_servo_right, pitch_servo_right_params);
    initServo(pitch_servo_left, pitch_servo_left_params);
    initServo(camber_servo_right, camber_servo_right_params);
    initServo(camber_servo_left, camber_servo_left_params);
}

void loop() {
    //Step 0: Read sensor values and serial input
    //Forces via Serial
    //target thrust and turn_torque via Serial
    //waterspeed via Sensor
    //microstate estimate via Serial
    //macrostate estimate via Serial
    //encoder readings via Sensor
    //Learning: takes 25 cycles to actually comprehend the value from serial input

    encoder_readings[0] = digitalRead(encoder_pin);
  
    // Print the state to the Serial Monitor
    //Serial.println(sensorState);

    //Step 1: Setpoint generation, input: waterspeed, thrust, target thrust, turn torque, target turn torque
    parameter_tuner(0, 0, 0, 0, 0, sine_params_ptr); //how should the sinewaves be like?, just mock values for now
    updateSineWaves(sine_params_ptr, targets_ptr, millis() - start_time); //generate the sinewaves

    // Step 2: State estimation
    bool heave_down = abs(encoder_readings[0]-1); //1 if obstructed, 0 if not
    // heave_down = 0; //DEBUG: remove this line
    estimate_servo_states(estimates_ptr, targets_ptr, heave_down); //should they all be in the format of servo angles ? if yes, preprocess encoder readings.

    //Step 3: control (control input is just the error?)
    //float target_speed_heave = setHeave(target_pos_heave);
    for (int i = 0; i < 5; i++){
        control[i] = (targets[i]-estimates[i])*Kp; //just a P controller for now
    }
    float write_heave = (control[0]*1)+targets[0];

    // Step 4: Write target values to servos

    //with feedback control (target values in this 2DOF control are target values + controller output)
    //different for servo 0 (heave) because it is a speed control
    heave_servo.writeMicroseconds(angletoPWM(write_heave)); //change back to target heave speed

    //as there is no feedback sensors yet, we just write the target values
    pitch_servo_right.write(targets[1]); // + control[1]);
    pitch_servo_left.write(targets[2]);// + control[2]);
    camber_servo_right.write(targets[3]);// + control[3]);
    camber_servo_left.write(targets[4]);// + control[4]);

    //DEBUG: printing targets, control and heave_down for i=0
    // Serial.print("heave_down, Target, estimate, control, write_heave: ");
    Serial.print("/*");        // Frame start sequence 
    Serial.print(heave_down);
    Serial.print(",");
    Serial.print(targets[0]);
    Serial.print(",");
    Serial.print(estimates[0]);
    Serial.print(",");
    Serial.print(control[0]);
    Serial.print(",");
    Serial.print(angletoPWM(write_heave));
    Serial.print("*/");        // Frame finish sequence 
    Serial.println();

}

float angletoPWM(float angle){
    return 500 + angle * 2000 / 270;
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
