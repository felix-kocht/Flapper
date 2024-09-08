/*
  Arduino Wireless Communication Tutorial
      Example 1 - Transmitter Code

  by Dejan Nedelkovski, www.HowToMechatronics.com

  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8, 4000000); // CE, CSN, SPI frequency
const byte address[6] = "00001";

// Define a structure to hold the data package
struct Data_Package {
  float values[7];
};

void read_serial_float();

Data_Package data = {0.0, 38, 60, 90, 0, 0, 0}; // Initialize with default values; (frequency, heave, pitch, camber, pitch_phase, camber_phase, turn_rate)


void setup() {
  Serial.begin(19200);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  read_serial_float(); // Read serial input and update data package
  radio.write(&data, sizeof(Data_Package)); // Send the whole data package to the receiver
  Serial.println("Sent data package");
  Serial.println(data.values[0]);
  Serial.println(data.values[1]);
    Serial.println(data.values[2]);
    Serial.println(data.values[3]);
    Serial.println(data.values[4]);
    Serial.println(data.values[5]);
    Serial.println(data.values[6]);

  delay(500); // Wait before the next transmission
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
    char* token = strtok(inputChars, ",");

    while (token != NULL && index < 7) { // Parse up to 7 float values
      data.values[index] = atof(token);
      token = strtok(NULL, ",");
      index++;
    }
  }
}