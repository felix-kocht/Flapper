#include <Arduino.h>
#include <Servo.h>
#include "setpoints.h"

//Changeable parameters:
const int SERVO_PINS[5] = {9, 8, 7, 6, 5}; // heave, pitch right, pitch left, camber right, camber left
const int NUM_PERIPHERALS = 5;
const int PERIPHERAL_PINS[NUM_PERIPHERALS] = {4, 3, 2, 99, 99}; //Pulley motor, Powersensor, PPM reciever, SD Card, Waterspeed Sensor...
const bool PERIPHERALS_CONNECTED[NUM_PERIPHERALS] = {false, false, false, false, false}; //same order as above 
//const bool PERIPHERALS_CONNECTED[NUM_PERIPHERALS] = {true, true, false, false, false}; //Lab configuration
//const bool PERIPHERALS_CONNECTED[NUM_PERIPHERALS] = {false, true, true, true, true}; //Lake configuration
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
void read_serial_float();

// Control variables
unsigned long start_time = 0;
unsigned long change_time = 0; //time when frequency was changed
float sine_params[4][5] = {0}; //4 parameters for sine wave (A,f,phase,offset), for all 5 servos,
float (*sine_params_ptr)[5] = sine_params; //pointer needed for use in funtions
float setpoints[5] = {0};
float (*setpoints_ptr) = setpoints; //pointer needed for use in funtions
float heave_lowpoint = 0; //initialized so that it always starts at the lower end of the amplitude

void setup() {
    // Initialize Serial communication
    Serial.begin(BAUD_RATE);  
    start_time = millis();
    change_time = start_time;
    
    // Initialize servos
    init_servos();

    // Initialize peripherals
    for (int i = 0; i < NUM_PERIPHERALS; i++) {
        if (PERIPHERALS_CONNECTED[i]) {
            //TODO: Initialize peripheral at relevant pin
        }
    }

    heave_lowpoint = get_minimum_heave();
    
    Serial.println("Setup complete");


}

void loop() {
    // Simulate sine wave parameters
    tune_parameters(sine_params_ptr);

    if (setpoints[0] < (heave_lowpoint + 1)) { //TODO: should not be hardcoded, could be a problem if not all 5 servos are in phase (?)
        read_serial_float();
    }

    
    update_sine_waves(sine_params_ptr, setpoints_ptr, millis() - change_time);
    //only reads once lower end of sine amplitude is reached


    // Write target values to servos (without feedback control for now)
    heave_servo.write(setpoints[0]);
    pitch_servo_right.write(setpoints[1]);
    pitch_servo_left.write(setpoints[2]);
    camber_servo_right.write(setpoints[3]);
    camber_servo_left.write(setpoints[4]);

    // Print values for debugging: Heave, Power consumption, noise
    float valuesToPrint[] = {setpoints[0], 0, 0};
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

void read_serial_float() {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n'); // Read the input until newline character
        input.trim(); // Remove any leading or trailing whitespace

        // Create a mutable copy of the input string
        char inputChars[input.length() + 1];
        input.toCharArray(inputChars, input.length() + 1);

        // Split the input string by commas
        int index = 0;
        float values[7] = {0.0,40,60,90,0,0,0}; // Adjust according to default values
        char* token = strtok(inputChars, ",");

        while (token != NULL && index < 5) { // Parse up to 5 float values
            values[index] = atof(token);
            token = strtok(NULL, ",");
            index++;
        }

        if (index > 0) {
            // Assuming the first float value is for changing frequency
            change_frequency(values[0]);

            // If there are more values, handle them as needed
            if (index > 1) {
                // Example: Use the second float value for another function
                change_rest(values[1], values[2], values[3], values[4], values[5], values[6]);
            }

            // Update heave_lowpoint and change_time
            heave_lowpoint = get_minimum_heave();
            change_time = millis();
        }
    }
}
