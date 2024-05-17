#include <Arduino.h>
#include "HX711.h"
#include "load_cell.h"

//TODO calibration mode or something (there is a seperate sketch for this)
//Calibration mode should print all 5 values nicely readable continuously (seperate code, filtered or not??... I think filtered is better --> then its quite similar to this, so maybe just a mode?)
//tutorial: https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide/all
//hx711 can take 2.7V to 5V

/*setup:
- cell 1: left over rail
- cell 2: right over rail
- cell 3: left on baseplate
- cell 4: right on baseplate
- cell 5: at the front of baseplate
- using 10JHz sampling rate for now, 80Hz can be used too */

//parameters: DAT, CLK, calibration_factor, zero_factor
const HX711Params cell_1_params = {2,3, -7050.0, 1};
const HX711Params cell_2_params = {4,5, -7050.0, 1};
const HX711Params cell_3_params = {6,7, -7050.0, 1};
const HX711Params cell_4_params = {8,9, -7050.0, 1};
const HX711Params cell_5_params = {10,11, -7050.0, 1};

//create hx711 instance
HX711 cell_1;
HX711 cell_2;
HX711 cell_3;
HX711 cell_4;
HX711 cell_5;

//function prototypes
void push_by_2();

//variables to hold the readings
float readings[5][6] = {0}; //for each of the 5 cells: y,x,y-1,x-1,y-2,x-2 

void setup() {
  // initialize serial communication at 57600 bits per second:
  SerialUSB.begin(9600);
  SerialUSB.println("Starting setup");
  // initialize each hx711 object
  load_cell_init(cell_1, cell_1_params);
  SerialUSB.println("Cell 1 initialized");
  load_cell_init(cell_2, cell_2_params);
  SerialUSB.println("Cell 2 initialized");
  load_cell_init(cell_3, cell_3_params);
  SerialUSB.println("Cell 3 initialized");
  load_cell_init(cell_4, cell_4_params);
  SerialUSB.println("Cell 4 initialized");
  load_cell_init(cell_5, cell_5_params);
  SerialUSB.println("Cell 5 initialized");
}

void loop() {
  // sample each hx711 object (for now just cell 1)
  readings[0][1] = cell_1.get_units();
  readings[1][1] = cell_2.get_units();
  readings[2][1] = cell_3.get_units();
  readings[3][1] = cell_4.get_units();
  readings[4][1] = cell_5.get_units();  
  // Serial.println(cell_1.read());                 // print a raw reading from the ADC, for debug
  // filter each sample?
  readings[0][0] = filter(readings[0][1], readings[0][2], readings[0][3], readings[0][4], readings[0][5]);
  readings[1][0] = filter(readings[1][1], readings[1][2], readings[1][3], readings[1][4], readings[1][5]);
  readings[2][0] = filter(readings[2][1], readings[2][2], readings[2][3], readings[2][4], readings[2][5]);
  readings[3][0] = filter(readings[3][1], readings[3][2], readings[3][3], readings[3][4], readings[3][5]);
  readings[4][0] = filter(readings[4][1], readings[4][2], readings[4][3], readings[4][4], readings[4][5]);
  // calculate the actual forces
  // print the values to the serial monitor
  // Serial.println(readings[0][1]); //for debug
  SerialUSB.println("Printing readings, left, right, base left, base right, front:");
  SerialUSB.println(readings[0][0]); //for debug
  SerialUSB.println(readings[1][0]); //for debug
  SerialUSB.println(readings[2][0]); //for debug
  SerialUSB.println(readings[3][0]); //for debug
  SerialUSB.println(readings[4][0]); //for debug
  push_by_2(); //shifts entire array by 2 (making x => x-1, x-1 => x-2, same for y)
  delay(1000);
}

void push_by_2() {
  //push all values in the array readings by 2 to the right
  for (int i = 0; i >= 0; i--) { //TODO change 0 to 4
    for (int j = 3; j >= 0; j--) {
      readings[i][j+2] = readings[i][j];
    }
  }
}
