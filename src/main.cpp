#include <Arduino.h>
#define onboardLED 13

// put function declarations here:
int myFunction(int x, int y);

void setup() {
  // put your setup code here, to run once:
  pinMode(onboardLED, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(onboardLED, HIGH);
  delay(1000);
  digitalWrite(onboardLED, LOW);
  delay(1000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}