#ifndef PPMREADER_H
#define PPMREADER_H
#include <Arduino.h>

#define PPM_PIN 2  // Replace with your actual PPM pin

extern volatile unsigned int ppmValues[4];
extern volatile byte currentChannel;

void setupPPM();
void ppmInterruptHandler();

#endif
