#include <Arduino.h>
#include <Servo.h>
#include "setpoints.h"

//Change these:
const int SERVO_PINS[5] = {9, 8, 7, 6, 5}; // heave, pitch right, pitch left, camber right, camber left
const int BAUD_RATE = 9600;
//End of changeable parameters

// Create servo instances
Servo heave_servo;
Servo pitch_servo_right;
Servo pitch_servo_left;
Servo camber_servo_right;
Servo camber_servo_left;

// Function prototypes
void init_servos();
void print_floats(float* values, int length);
float read_serial_float();

// Control variables
unsigned long start_time = 0;
float sine_params[4][5] = {0}; //4 parameters for sine wave (A,f,phase,offset), for all 5 servos,
float (*sine_params_ptr)[5] = sine_params; //pointer needed for use in funtions
float setpoints[5] = {0};
float (*setpoints_ptr) = setpoints; //pointer needed for use in funtions

void setup() {
    // Initialize Serial communication
    Serial.begin(BAUD_RATE);  
    start_time = millis();
    
    // Initialize servos
    init_servos();
    
    Serial.println("Setup complete");
}

void loop() {
    // Simulate sine wave parameters
    
    tune_parameters(sine_params_ptr);
    update_sine_waves(sine_params_ptr, setpoints_ptr, millis() - start_time);
    //only reads once lower end of sine amplitude is reached
    if (setpoints[1] < 11) { //TODO: should not be hardcoded, could be a problem if not all 5 servos are in phase (?)
        read_serial_float();
    }

    // Write target values to servos (without feedback control for now)
    heave_servo.write(setpoints[0]);
    pitch_servo_right.write(setpoints[1]);
    pitch_servo_left.write(setpoints[2]);
    camber_servo_right.write(setpoints[3]);
    camber_servo_left.write(setpoints[4]);

    // Print values for debugging
    float valuesToPrint[] = { setpoints[0], setpoints[1], setpoints[2], setpoints[3], setpoints[4], (float)millis() - (float)start_time };
    int length = sizeof(valuesToPrint) / sizeof(valuesToPrint[0]);
    print_floats(valuesToPrint, length);
}

void init_servos() {
    heave_servo.attach(SERVO_PINS[0]);  // Attach heave_servo to pin 9
    pitch_servo_right.attach(SERVO_PINS[1]);  // Attach right pitch servo to pin 8
    pitch_servo_left.attach(SERVO_PINS[2]);  // Attach left pitch servo to pin 7
    camber_servo_right.attach(SERVO_PINS[3]);  // Attach right camber servo to pin 6
    camber_servo_left.attach(SERVO_PINS[4]);  // Attach left camber servo to pin 5
}

void print_floats(float* values, int length) {
    Serial.print("/*");  // Frame start sequence
    for (int i = 0; i < length; ++i) {
        Serial.print(values[i]);
        if (i < length - 1) {
            Serial.print(",");
        }
    }
    Serial.print("*/");  // Frame finish sequence
    Serial.println();
}

float read_serial_float(){
    if(Serial.available() > 0){
        int time_before = millis() - start_time;
        float new_frequency = Serial.parseFloat(); 
        changeFrequency(new_frequency, time_before);
    }
}
