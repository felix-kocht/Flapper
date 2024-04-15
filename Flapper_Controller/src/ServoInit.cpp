#include "ServoInit.h"

void initServoAngle(Servo &servo, const ServoParams &params);
void initServoSpeed(Servo &servo, const ServoParams &params);

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