#include <Arduino.h>

//create the 5 hx711 objects
//tutorial: https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide/all

//TODO calibration mode or something

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // initialize each hx711 object
}

void loop() {
  // sample each hx711 object
  // filter each sample?
  // calculate the actual forces
  // print the values to the serial monitor
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}