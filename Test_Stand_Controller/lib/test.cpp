#include "Arduino.h"

void setup() {
  // initialize serial communication at 9600 bits per second:
  SerialUSB.begin(9600);
  //while (!Serial);  // Wait for the serial port to connect (only needed on native USB ports)
  SerialUSB.println("Starting setup");
}

void loop() {
  // Just a minimal test to see if the loop works
  SerialUSB.println("Looping");
  delay(1000);
}
