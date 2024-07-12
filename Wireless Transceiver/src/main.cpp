/*
  Arduino Wireless Communication Tutorial
        Example 1 - Receiver Code

  by Dejan Nedelkovski, www.HowToMechatronics.com

  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  float frequency = 3.1;
  float heave = 3.1;
  float pitch = 3.1;
  float camber = 3.1;
  float pitch_phase = 3.1;
};

Data_Package data; //Create a variable with the above structure

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  // Check whether there is data to be received
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
  }
  // Print the received data
  Serial.print(" Frequency: ");
  Serial.print(data.frequency);
  Serial.print(" Heave: ");
  Serial.print(data.heave);
  Serial.print(" Pitch: ");
  Serial.print(data.pitch);
  Serial.print(" Camber: ");
  Serial.print(data.camber);
  Serial.print(" Pitch Phase: ");
  Serial.print(data.pitch_phase);
  Serial.println();
}