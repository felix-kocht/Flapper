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

// Function to initialize a servo with specific parameters
void initServo(Servo &servo, const ServoParams &params);

#endif
