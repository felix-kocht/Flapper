#include <Arduino.h>
#include <Math.h>   
#include <Servo.h>  
#include "ServoInit.h"  
#include "setpoints.h"  
#include "Powerchip.h"
#include "ppmreader.h"

//Wiring: heave: 9, pitch right: 8, pitch left: 7, camber right: 6, camber left: 5, encoder: 4, PPM: 2

//function prototypes
float SerialRead();
void initPeripherals();
void initServos();
void printFloats(float* values, int length);
void useRC();

//peripheral flags
bool encoder_present = false;
bool rc_reciever_present = false;
bool power_sensor_present = false;

//control variables, some of these are obsolete, if no real controller is used
const int encoder_pin = 4; //encoder pin
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
float average_consumption = 0; //average power consumption
int maincounter = 0; //counter for average consumption
float frequency_adjustment = 0.0; //Hz, ca. 0.875Hz per m/s

void setup() {
    // Initialize serial communication
    Serial.begin(9600);  
    Serial.println("Starting setup");
    start_time = millis();
    
    // Initialize servos
    initServos(); //heave, pitch right, pitch left, camber right, camber left

    // Initialize peripherals, if present
    initPeripherals(); //encoder, power sensor, PPM receiver (add more in the function below if needed)

    Serial.println("Setup complete");
}

void loop() {
    //Step 0: Read sensor values and serial input
    if (encoder_present) {
        encoder_readings[0] = digitalRead(encoder_pin);
    }
    if (power_sensor_present) {
        float consumption = getPower();
        if(maincounter > 1000){ //average over last 1000 values, unless there are less than 1000 values
            average_consumption = (average_consumption*1000 + consumption)/(1000+1); 
        }else{
        average_consumption = (average_consumption*maincounter + consumption)/(maincounter+1); 
        }
    }

    //Step 1: Setpoint generation, input: waterspeed, thrust, target thrust, turn torque, target turn torque
    parameter_tuner(0, 0, 0, 0, 0, sine_params_ptr); //how should the sinewaves be like?, just mock values for now

    if (rc_reciever_present) {
        useRC(); //use radio control to adjust sine parameters
    }

    updateSineWaves(sine_params_ptr, targets_ptr, millis() - start_time); //generate the sinewaves

    // Step 3: Write target values to servos (without feedback control for now)
    heave_servo.write(targets[0]);// + control[0]);
    pitch_servo_right.write(targets[1]); // + control[1]);
    pitch_servo_left.write(targets[1]);// + control[2]);
    camber_servo_right.write(targets[3]);// + control[3]);
    camber_servo_left.write(targets[4]);// + control[4]);

    //Print values
    float valuesToPrint[] = {
        0.0, //placeholder for heave_down,
        targets[0], 
        estimates[0], 
        sine_params_ptr[0][1],
        average_consumption
    };
    int length = sizeof(valuesToPrint) / sizeof(valuesToPrint[0]);
    printFloats(valuesToPrint, length);

    maincounter++;
}

void initPeripherals(){
    // Check for Encoder
    pinMode(encoder_pin, INPUT);
    if (digitalRead(encoder_pin) == HIGH || digitalRead(encoder_pin) == LOW) {
        encoder_present = true;  // Simple check if the pin is responsive
        Serial.println("Encoder detected.");
    } else {
        Serial.println("Encoder not detected.");
    }

    // Check for INA219 Power Sensor
    if (setupINA219()) {
        power_sensor_present = true;
        Serial.println("INA219 power sensor detected.");
    } else {
        Serial.println("INA219 power sensor not detected.");
    }

    // Check for PPM Receiver
    //setup PPM reciever at PIN 2, it will read automatically, using interrupts
    if (digitalRead(PPM_PIN) == HIGH || digitalRead(PPM_PIN) == LOW) {
        rc_reciever_present = true;
        Serial.println("PPM receiver detected.");
    } else {
        Serial.println("PPM receiver not detected.");
    }
}

void initServos(){
    // Initialize Servos
    initServo(heave_servo, heave_servo_params);
    initServo(pitch_servo_right, pitch_servo_right_params);
    initServo(pitch_servo_left, pitch_servo_left_params);
    initServo(camber_servo_right, camber_servo_right_params);
    initServo(camber_servo_left, camber_servo_left_params);
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

void useRC(){
    // changing sine params based on radio input
    sine_params_ptr[1][0] = (ppmValues[2]-1000)/10;    // Pitch Amplitude 
    sine_params_ptr[2][0] = (ppmValues[2]-1000)/10;    // Pitch Amplitude 
    //adjusting frequency based on radio input
    if(ppmValues[1] < 1400 || ppmValues[1] > 1648){
        frequency_adjustment = frequency_adjustment + double(int(ppmValues[1])-1524)/9000.0; // Frequency f
        //limit frequency adjustment to 0 to 1.5 Hz
        if(frequency_adjustment > 1.5){
            frequency_adjustment = 1.5;
        }else if(frequency_adjustment < 0){
            frequency_adjustment = 0;
        }
    }
    sine_params_ptr[0][1] = sine_params_ptr[0][1] + frequency_adjustment; // Frequency f 
    sine_params_ptr[1][1] = sine_params_ptr[1][1] + frequency_adjustment;  // Frequency f
    sine_params_ptr[2][1] = sine_params_ptr[2][1] + frequency_adjustment;  // Frequency f
    sine_params_ptr[3][1] = sine_params_ptr[3][1] + frequency_adjustment;  // Frequency f
    sine_params_ptr[4][1] = sine_params_ptr[4][1] + frequency_adjustment;  // Frequency f
}

/* Code for later use:
   //with feedback control (target values in this 2DOF control are target values + controller output)
    heave_servo.writeMicroseconds(angletoPWM(targets[0]+control[0])); //angletoPWM necessary because this servo needs pwm values

    float angletoPWM(float angle){
    return 500 + angle * 2000 / 270;
}
    //Step 2: state estimation
    bool heave_down = abs(encoder_readings[0]-1); //1 if obstructed, 0 if not
    heave_down = 0; //DEBUG: remove this line if using sensor
    estimate_servo_states(estimates_ptr, targets_ptr, heave_down); //should they all be in the format of servo angles ? if yes, preprocess encoder readings.

    //Step 3: control (control input is just the error?)
    //float target_speed_heave = setHeave(target_pos_heave);
    for (int i = 0; i < 5; i++){
        control[i] = (targets[i]-estimates[i])*Kp; //just a P controller for now
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
//is this even useful (only one instance)?
float SerialRead(){
    if(Serial.available() > 0){
        return Serial.parseFloat(); 
    }else{
        return 0;
    }
}
     */