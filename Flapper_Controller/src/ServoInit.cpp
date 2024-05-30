#include "ServoInit.h"

void initServoAngle(Servo &servo, const ServoParams &params);
void initServoSpeed(Servo &servo, const ServoParams &params);

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

void initServo(Servo &servo, const ServoParams &params){
    servo.attach(params.pin);    // Attach servo to specified pin
    if (params.mode == false){
        initServoAngle(servo, params);
    } else {
        initServoSpeed(servo, params);
    }
}

//for standard servo (with angle control)
void initServoAngle(Servo &servo, const ServoParams &params) {
    float initAngle = params.initAngle;    // Set initial angle
    if (params.initAngle < params.minAngle) {
        initAngle = params.minAngle;    // Ensure initial angle is not smaller than min angle
    } else if (params.initAngle > params.maxAngle) {
        initAngle = params.maxAngle;    // Ensure initial angle is not greater than max angle
    }
    servo.write(initAngle); // Write initial angle
}

//for continuous servo (with speed control)
void initServoSpeed(Servo &servo, const ServoParams &params) {
   float initAngle = params.initAngle;    // Set initial speed
    if (params.initAngle < params.minAngle) {
        initAngle = params.minAngle;    // Ensure initial speed is not smaller than min speed
    } else if (params.initAngle > params.maxAngle) {
        initAngle = params.maxAngle;    // Ensure initial speed is not greater than max speed
    }
    servo.writeMicroseconds(initAngle); // Write initial speed
}