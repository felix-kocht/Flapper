#ifndef LOAD_CELL_H
#define LOAD_CELL_H
#include "HX711.h" 

//struct to hold the parameters for the hx711 object
struct HX711Params {
  int DOUT; 
  int CLK; 
  float calibration_factor; 
  long zero_factor;
};

void load_cell_init(HX711 cell,HX711Params hx711_params);

float get_Fx(float reading_1, float reading_2);
float get_Mz(float reading_1, float reading_2);
float get_Fz(float reading_3, float reading_4, float reading_5);
float get_Mx(float reading_3, float reading_4, float reading_5);
float get_My(float reading_3, float reading_4, float reading_5);

float filter(float x, float y_1, float x_1, float y_2, float x_2); //cutoff at 2Hz, 2nd order butterworth filter (change in load_cell.cpp if using 80Hz sampling rate)


#endif