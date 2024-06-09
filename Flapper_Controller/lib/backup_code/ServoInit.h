#ifndef SERVO_INIT_H
#define SERVO_INIT_H
#include <Servo.h> // Include servo library

// Define struct to hold servo parameters
struct ServoParams {
    bool mode;      // Servo mode (0 = angle, 1 = speed)
    int pin;       // Servo pin
    int minAngle;  // Minimum angle
    int initAngle;  // initial angle
    int maxAngle;  // Maximum angle
};

// extern ServoParams heave_servo_params; // Heave servo parameters
// extern ServoParams pitch_servo_right_params; // Pitch servo right parameters
// extern ServoParams pitch_servo_left_params; // Pitch servo left parameters
// extern ServoParams camber_servo_right_params; // Camber servo right parameters
// extern ServoParams camber_servo_left_params; // Camber servo left parameters

// extern Servo heave_servo; // Heave servo instance
// extern Servo pitch_servo_right; // Pitch servo right instance
// extern Servo pitch_servo_left; // Pitch servo left instance
// extern Servo camber_servo_right; // Camber servo right instance
// extern Servo camber_servo_left; // Camber servo left instance

// Function to initialize a servo with specific parameters
void initServo(Servo &servo, const ServoParams &params);

#endif
