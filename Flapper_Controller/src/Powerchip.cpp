#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Powerchip.h>
 
Adafruit_INA219 ina219;
 
bool setupINA219()
{
  Serial.begin(9600);
  while (!Serial) 
  {
    delay(1);
  }
 
    ina219.begin();
  // Initialize the INA219.
  if (! ina219.begin())
  {
    return false;
  }else{
    return true;
  }
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range, call:
  //ina219.setCalibration_16V_400mA();
}
 
float getPower()
{
/*   float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0; */
  float power_mW = 0;
 
  /* shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA(); */
  power_mW = ina219.getPower_mW();
 /*  loadvoltage = busvoltage + (shuntvoltage / 1000);
 
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println(""); */
  return power_mW;
}