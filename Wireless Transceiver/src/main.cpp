/*
  Arduino Wireless Communication Tutorial
        Example 1 - Receiver Code

  by Dejan Nedelkovski, www.HowToMechatronics.com

  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/

  Tutorial: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
  Common issues: https://github.com/nRF24/RF24/blob/master/COMMON_ISSUES.md
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte addresses[][6] = {"00001","00002"};

// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  float frequency = 0.0;
  float heave = 0.0;
  float pitch = 0.0;
  float camber = 0.0;
  float pitch_phase = 0.0;
};

Data_Package data; //Create a variable with the above structure

void setup() {
  Serial.begin(9600);
  radio.begin();
  // at the Receiver
  radio.openWritingPipe(addresses[0]); // 00002
  radio.openReadingPipe(1, addresses[1]); // 00001
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {

  delay(5);

  radio.stopListening();
  int angleValue = map(2, 0, 1023, 0, 180);
  radio.write(&angleValue, sizeof(angleValue));

  delay(5);
  radio.startListening();
  while (!radio.available());
  radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
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