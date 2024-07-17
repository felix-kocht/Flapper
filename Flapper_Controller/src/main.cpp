#include <Arduino.h>
#include <Servo.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "setpoints.h"
#include "powerchip.h"
#include <Adafruit_GPS.h>

// what's the name of the hardware serial port?
#define GPSSerial Serial1

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO false

uint32_t timer = millis();

//run pio run --target clean before building, it doesnt work otherwise

//Changeable parameters:
const int SERVO_PINS[5] = {9, 8, 7, 6, 5}; // heave, pitch right, pitch left, camber right, camber left
const int NUM_PERIPHERALS = 5;
const int PERIPHERAL_PINS[NUM_PERIPHERALS] = {4, 3, 99, 99, 99}; //Pulley motor, Powersensor, radio, SD Card (11,12,13), GPS
const bool PERIPHERALS_CONNECTED[NUM_PERIPHERALS] = {false, true, true, false, true}; //same order as above 
//const bool PERIPHERALS_CONNECTED[NUM_PERIPHERALS] = {true, true, false, false, false}; //Lab configuration
//const bool PERIPHERALS_CONNECTED[NUM_PERIPHERALS] = {false, true, true, true, true}; //Lake configuration
const long int BAUD_RATE = 115200;
//End of changeable parameters
RF24 radio(7, 8); // CE, CSN

const byte addresses[][6] = {"00001","00002"};

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
void init_peripheral(int i);
void init_radio();
void init_gps();
void loop_gps();

// Control variables
unsigned long start_time = 0;
unsigned long change_time = 0; //time when frequency was changed
float sine_params[4][5] = {0}; //4 parameters for sine wave (A,f,phase,offset), for all 5 servos,
float (*sine_params_ptr)[5] = sine_params; //pointer needed for use in funtions
float setpoints[5] = {0};
float (*setpoints_ptr) = setpoints; //pointer needed for use in funtions
float heave_lowpoint = 0; //initialized so that it always starts at the lower end of the amplitude

//for wireless sending
struct Data_Package_1 {
  float power = 0.0;
  float posx = 0.0;
  float posy = 0.0;
  float speed = 0.0;
  float gps_fine = 0.0;
}; 
Data_Package_1 outgoing_data;

//for wireless reading
struct Data_Package {
  float frequency = 0.0;
  float heave = 0.0;
  float pitch = 0.0;
  float camber = 0.0;
  float pitch_phase = 0.0;
};
Data_Package incoming_data;

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
            init_peripheral(i);
        }
    }

    heave_lowpoint = get_minimum_heave();
    
    Serial.println("Setup complete");
}

void loop() {
    float power_reading = 0;
    // Simulate sine wave parameters
    tune_parameters(sine_params_ptr);

    // Read serial input at every loop
    if (setpoints[0] < (heave_lowpoint + 1)) {
        read_serial_float();
    }

    //Use peripherals //TODO: add others in a structured way
    if (PERIPHERALS_CONNECTED[1]) {
            power_reading = getPower();
        }

    update_sine_waves(sine_params_ptr, setpoints_ptr, millis() - change_time);
    //only reads once lower end of sine amplitude is reached

    // Write target values to servos (without feedback control for now)
    heave_servo.write(setpoints[0]);
    pitch_servo_right.write(setpoints[1]+5); //correction factor
    pitch_servo_left.write(setpoints[2]-3);
    camber_servo_right.write(setpoints[3]);
    camber_servo_left.write(setpoints[4]);

    // Print values for debugging: Heave, Power consumption
    float valuesToPrint[] = {setpoints[0], power_reading,0,0,0};
    int length = sizeof(valuesToPrint) / sizeof(valuesToPrint[0]);
    //print_floats(valuesToPrint, length);
    if (PERIPHERALS_CONNECTED[2]){
        delay(5);
        radio.stopListening();
        outgoing_data.power = power_reading;
        radio.write(&outgoing_data, sizeof(Data_Package_1));

        delay(5);
        radio.startListening();
        while (!radio.available());
        radio.read(&incoming_data, sizeof(Data_Package));
        //Serial.println(data.frequency);
    }
    if (PERIPHERALS_CONNECTED[4]){
        loop_gps();
    }
    delay(20);
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

void init_peripheral(int i) {
    switch (i) {
        case 0:
            // Initialize pulley motor
            break;
        case 1:
            setupINA219();
            break;
        case 2:
            init_radio();
            break;
        case 3:
            // Initialize SD card
            break;
        case 4:
            init_gps();
            break;
        default:
            break;
    }
}

void init_radio() {
    radio.begin();
    radio.openWritingPipe(addresses[1]); // 00001
    radio.openReadingPipe(1, addresses[0]); // 00002
    radio.setPALevel(RF24_PA_MIN);
    radio.stopListening();
}

void init_gps()
{
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
}

void loop_gps() // run over and over again
{
  // read data from the GPS in the 'main loop'
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) Serial.print(c);
  // if a sentence is received, we can check the checksum, parse it...
  if (GPS.newNMEAreceived()) {
    // a tricky thing here is if we print the NMEA sentence, or data
    // we end up not listening and catching other sentences!
    // so be very wary if using OUTPUT_ALLDATA and trying to print out data
    Serial.print(GPS.lastNMEA()); // this also sets the newNMEAreceived() flag to false
    if (!GPS.parse(GPS.lastNMEA())) // this also sets the newNMEAreceived() flag to false
      return; // we can fail to parse a sentence in which case we should just wait for another
  }

    //approximately every 0.5 seconds or so, save GPS to datastructure
    if (millis() - timer > 500) {
    outgoing_data.gps_fine = GPS.fix;
    outgoing_data.posx = GPS.longitude;
    outgoing_data.posy = GPS.latitude;
    outgoing_data.speed = GPS.speed;

    Serial.print("GPS: ");
    Serial.print(outgoing_data.gps_fine);
    Serial.print(" ");
    Serial.print(outgoing_data.posx);
    Serial.print(" ");
    Serial.print(outgoing_data.posy);
    Serial.print(" ");
    Serial.print(outgoing_data.speed);
    Serial.println();
    timer = millis(); // reset the timer
    }
/* 
  // approximately every 2 seconds or so, print out the current stats
  if (millis() - timer > 2000) {
    timer = millis(); // reset the timer
    Serial.print("\nTime: ");
    if (GPS.hour < 10) { Serial.print('0'); }
    Serial.print(GPS.hour, DEC); Serial.print(':');
    if (GPS.minute < 10) { Serial.print('0'); }
    Serial.print(GPS.minute, DEC); Serial.print(':');
    if (GPS.seconds < 10) { Serial.print('0'); }
    Serial.print(GPS.seconds, DEC); Serial.print('.');
    if (GPS.milliseconds < 10) {
      Serial.print("00");
    } else if (GPS.milliseconds > 9 && GPS.milliseconds < 100) {
      Serial.print("0");
    }
    Serial.println(GPS.milliseconds);
    Serial.print("Date: ");
    Serial.print(GPS.day, DEC); Serial.print('/');
    Serial.print(GPS.month, DEC); Serial.print("/20");
    Serial.println(GPS.year, DEC);
    Serial.print("Fix: "); Serial.print((int)GPS.fix);
    Serial.print(" quality: "); Serial.println((int)GPS.fixquality);
    if (GPS.fix) {
      Serial.print("Location: ");
      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
      Serial.print(", ");
      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
      Serial.print("Angle: "); Serial.println(GPS.angle);
      Serial.print("Altitude: "); Serial.println(GPS.altitude);
      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
      Serial.print("Antenna status: "); Serial.println((int)GPS.antenna);
 */    
}
