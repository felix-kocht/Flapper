#include <Arduino.h>

// Pin where the YF-B3 is connected
const int flowSensorPin = 2;  // Example: Digital Pin 2

// Variables to store the pulse count and flow rate
volatile int pulseCount = 0;
float flowRate = 0.0;

// Calibration factor (example value, check the sensor's datasheet)
const float calibrationFactor = 4.5;

// ISR for counting pulses
void pulseCounter() {
    pulseCount++;
}

void setup() {
    // Initialize the serial monitor for output
    Serial.begin(9600);
    
    // Set up the flow sensor pin as an interrupt
    pinMode(flowSensorPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, FALLING);
}

void loop() {
    // Time interval for flow rate calculation (1 second)
    unsigned long interval = 1000;
    static unsigned long lastTime = 0;

    // Every second, calculate the flow rate
    if (millis() - lastTime >= interval) {
        lastTime = millis();

        // Convert pulse count to flow rate in liters per minute
        float litersPerMinute = (pulseCount / calibrationFactor) / (interval / 1000.0);
        
        // Print the flow rate to the serial monitor
        Serial.print("Flow rate: ");
        Serial.print(litersPerMinute*100);
        Serial.println(" L/min");
        
        // Reset the pulse count for the next interval
        pulseCount = 0;
    }
}
