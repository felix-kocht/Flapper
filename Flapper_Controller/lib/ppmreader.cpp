#include "PPMReader.h"
#include <Arduino.h>

//rc reciever

volatile unsigned int ppmValues[4] = {0, 0, 0, 0};
volatile byte currentChannel = 0;
volatile unsigned long lastInterruptTime = 0;

void setupPPM() {
  pinMode(PPM_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PPM_PIN), ppmInterruptHandler, FALLING);
}

void ppmInterruptHandler() {
  unsigned long currentTime = micros();
  unsigned long pulseLength = currentTime - lastInterruptTime;
  lastInterruptTime = currentTime;

  if (pulseLength > 3000) {  // Sync pulse (start of a new PPM frame)
    currentChannel = 0;
  } else {
    if (currentChannel < 4) {
      ppmValues[currentChannel] = pulseLength;
      currentChannel++;
    }
  }
}
