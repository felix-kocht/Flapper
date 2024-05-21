#include <Arduino.h>

#define PPM_PIN 2  // Replace with your actual PPM pin

volatile unsigned long lastInterruptTime;
volatile unsigned int ppmValues[4];  // Assuming 4 channels
volatile byte currentChannel = 0;

void ppmInterrupt();

void setup() {
  Serial.begin(9600);
  pinMode(PPM_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(PPM_PIN), ppmInterrupt, FALLING);
}

void loop() {
  // Print the channel values for debugging
  for (int i = 0; i < 4; i++) {
    Serial.print("Channel ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.println(ppmValues[i]);
  }
  delay(1000);  // Adjust delay as needed
}

void ppmInterrupt() {
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
