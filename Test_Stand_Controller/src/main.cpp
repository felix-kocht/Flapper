#include <Arduino.h>
#include <HX711.h>
#include "load_cell.h"

//TODO calibration mode or something (there is a seperate sketch for this)
//tutorial: https://learn.sparkfun.com/tutorials/load-cell-amplifier-hx711-breakout-hookup-guide/all
//hx711 can take 2.7V to 5V

/*setup:
- cell 1: left over rail
- cell 2: right over rail
- cell 3: left on baseplate
- cell 4: right on baseplate
- cell 5: at the back of baseplate
- using 10JHz sampling rate for now, 80Hz can be used too */

//parameters: DAT, CLK, calibration_factor, zero_factor
HX711Params cell_1_params = {3,2, -7050.0, 1};

//create hx711 instance
HX711 cell_1;

//function prototypes
void push_by_2();

//variables to hold the readings
float readings[1][6]; //for each of the 5 cells: y,x,y-1,x-1,y-2,x-2 
//TODO change 1 to 5

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  Serial.println("Starting setup");
  // initialize each hx711 object
  load_cell_init(cell_1, cell_1_params);
}

void loop() {
  // sample each hx711 object
  readings[0][1] = cell_1.get_units();
  // filter each sample?
  readings[0][0] = filter(readings[0][1], readings[0][2], readings[0][3], readings[0][4], readings[0][5]);
  // calculate the actual forces
  // print the values to the serial monitor
  Serial.println(readings[0][1]); //for debug
  Serial.println(readings[0][0]); //for debug
  push_by_2(); //shifts entire array by 2 (making x => x-1, x-1 => x-2, same for y)
  delay(100);
}

void push_by_2() {
  //push all values in the array readings by 2 to the right
  for (int i = 4; i >= 0; i--) {
    for (int j = 5; j >= 0; j--) {
      readings[i+1][j] = readings[i][j];
    }
  }
}