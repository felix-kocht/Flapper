#include <Arduino.h>
#include <Math.h>   
#include <Servo.h>  
#include "setpoints.h"  
// #include "Powerchip.h"
// #include "ppmreader.h"

//Wiring: heave: 9, pitch right: 8, pitch left: 7, camber right: 6, camber left: 5, encoder: 4, PPM reciever: 2
//Change all Serial back to Serial, when going back to an Arduino board

// Create servo instances
const int servopins[5] = {9, 8, 7, 6, 5}; //heave, pitch right, pitch left, camber right, camber left
Servo heave_servo;
Servo pitch_servo_right;
Servo pitch_servo_left;
Servo camber_servo_right;
Servo camber_servo_left;

//function prototypes
// void initPeripherals();
void initServos();
void printFloats(float* values, int length);
//void useRC();
// float angletoPWM(float angle);
// void serial_frequency();

// //peripheral flags
// bool encoder_present = false;
// bool rc_reciever_present = false;
// bool power_sensor_present = false;

//control variables, some of these are obsolete, if no real controller is used
const int encoder_pin = 4; //encoder pin
unsigned long start_time = 0;
float sine_params[4][5] = {0}; //for all 5 servos, operational parameters for sine wave generation (A,f,phase,offset)
float (*sine_params_ptr)[5] = sine_params; //pointer needed for use in funtions
float targets[5] = {0}; //for all 5 servos, target angle
float (*targets_ptr) = targets; //pointer needed for use in funtions
bool encoder_readings[2] = {0}; //for all 2 encoders, on or off
float average_consumption = 0; //average power consumption
int maincounter = 0; //counter for average consumption

void setup() {
    // Initialize Serial communication
    Serial.begin(9600);  
    Serial.println("Starting setup");
    start_time = millis();
    
    // Initialize servos
    initServos(); //heave, pitch right, pitch left, camber right, camber left

    // Initialize peripherals, if present
    //initPeripherals(); //encoder, power sensor, PPM receiver (add more in the function below if needed)

    Serial.println("Setup complete");
}

void loop() {
    //Step 0: Read sensor values and Serial input
    // if (encoder_present) {
    //     encoder_readings[0] = digitalRead(encoder_pin);
    // }
    // if (power_sensor_present) {
    //     float consumption = getPower();
    //     if(maincounter > 1000){ //average over last 1000 values, unless there are less than 1000 values
    //         average_consumption = (average_consumption*1000 + consumption)/(1000+1); 
    //     }else{
    //     average_consumption = (average_consumption*maincounter + consumption)/(maincounter+1); 
    //     }
    // }

    //Step 1: Setpoint generation, input: waterspeed, thrust, target thrust, turn torque, target turn torque
    parameter_tuner(0, 0, 0, 0, 0, sine_params_ptr); //how should the sinewaves be like?, just mock values for now

    // if (rc_reciever_present) {
    //     useRC(); //use radio control to adjust sine parameters
    //     //serial_frequency(); //DEBUG
    // }

    updateSineWaves(sine_params_ptr, targets_ptr, millis() - start_time); //generate the sinewaves

    // Step 3: Write target values to servos (without feedback control for now)
    heave_servo.write(targets[0]);// + control[0]);
    pitch_servo_right.write(targets[1]);//targets[1]); // + control[1]);
    pitch_servo_left.write(targets[2]);// + control[2]);
    // camber_servo_right.write(targets[3]);// + control[3]);
    // camber_servo_left.write(targets[4]);// + control[4]);  

    // Print values: 0,targets, pitch angle, frequency, phase, time
    float valuesToPrint[] = { 0.0, targets[2], sine_params_ptr[1][0], sine_params_ptr[0][1], general_phase, (float)millis() - (float)start_time};
    int length = sizeof(valuesToPrint) / sizeof(valuesToPrint[0]);
    printFloats(valuesToPrint, length);

    maincounter++;
}

// void initPeripherals(){ //TODO: Check if this works on the arduino as intended
//     // Check for Encoder
//     pinMode(encoder_pin, INPUT); //TODO: is this a PULLDOWN?
//     if (digitalRead(encoder_pin) == LOW) {
//         pinMode(encoder_pin, INPUT_PULLUP);
//         if(digitalRead(encoder_pin) == HIGH){
//             Serial.println("Encoder not detected.");
//             } else {
//         Serial.println("Encoder detected.");
//         encoder_present = true;  // Simple check if the pin is responsive
//             }
//     } else {
//         Serial.println("Encoder detected.");
//         encoder_present = true;  // Simple check if the pin is responsive
//     }

//     // Check for INA219 Power Sensor
//     if (setupINA219()) {
//         power_sensor_present = true;
//         Serial.println("INA219 power sensor detected.");
//     } else {
//         Serial.println("INA219 power sensor not detected.");
//     }

//     // Check for PPM Receiver
//     //setup PPM reciever at PIN 2, it will read automatically, using interrupts
    
//     if (true){ //digitalRead(PPM_PIN) == HIGH || digitalRead(PPM_PIN) == LOW) {
//         rc_reciever_present = true;
//         setupPPM();
//         Serial.println("PPM receiver detected.");
//     } else {
//         Serial.println("PPM receiver not detected.");
//     }
// }

void initServos(){
    // Initialize Servos
    heave_servo.attach(servopins[0]);  // Attach heave_servo to pin 9
    pitch_servo_right.attach(servopins[1]);  // Attach right pitch servo to pin 8
    pitch_servo_left.attach(servopins[2]);  // Attach left pitch servo to pin 7
    camber_servo_right.attach(servopins[3]);  // Attach right camber servo to pin 6
    //camber_servo_left.attach(servopins[4]);  // Attach left camber servo to pin 5
}

void printFloats(float* values, int length) { //TODO: try if this works
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

// void useRC(){
//     // changing sine params based on radio input
//     sine_params_ptr[1][0] = (ppmValues[2]-1000)/10;    // Pitch Amplitude 
//     sine_params_ptr[2][0] = (ppmValues[2]-1000)/10;    // Pitch Amplitude 
//     //adjusting frequency based on radio input
//     if(ppmValues[1] < 1400 || ppmValues[1] > 1648){
//         float frequency_adjustment = frequency_adjustment + double(int(ppmValues[1])-1524)/9000.0; // Frequency f
//         //limit frequency adjustment to 0 to 1.5 Hz
//         if(frequency_adjustment > 1.5){
//             frequency_adjustment = 1.5;
//         }else if(frequency_adjustment < 0){
//             frequency_adjustment = 0;
//         }
//         changeFrequency(frequency_adjustment, millis() - start_time);
//     }
// }

// float angletoPWM(float angle) {
//     return map(angle, 0, 180, 544, 2400); // Corrected PWM range
// }

// void serial_frequency(){
//     if(Serial.available() > 0){
//         int time_before = millis() - start_time;
//         float new_frequency = Serial.parseFloat(); 
//         changeFrequency(new_frequency, time_before);
//     }
// }
