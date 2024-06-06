#include "load_cell.h"

//distances between the load cells, assuming that the screwpoint is the relevant point on the front cells and the center relevant on the bottom cells 
const float WIDTH_FRONT = 0.105; //meters
const float LENGTH = 0.099; //meters
const float WIDTH_BOTTOM = 0.179; //meters

//function to initialize the load cell
void load_cell_init(HX711& cell,HX711Params hx711_params) {
  //initialize the hx711 object
  cell.begin(hx711_params.DOUT, hx711_params.CLK);
  //set the scale
  cell.set_scale(hx711_params.calibration_factor);
  //tare the scale
  cell.tare(hx711_params.zero_factor);
}

float filter(float x, float y_1, float x_1, float y_2, float x_2) {
    // Given filter coefficients
    const float a0 = 1.0;
    const float a1 = 2.0;
    const float a2 = 1.0;
    const float b0 = 4.841;
    const float b1 = 1.789;
    const float b2 = -0.948;

    // Calculate the output
    float y = (a0 * x + a1 * x_1 + a2 * x_2 - b1 * y_1 - b2 * y_2) / b0;

    return y;
}

float get_Fx(float reading_1, float reading_2) {
  return reading_1 + reading_2;
}

float get_Mz(float reading_1, float reading_2) {
  return (reading_1 - reading_2)*WIDTH_FRONT;
}

float get_Fz(float reading_3, float reading_4, float reading_5) {
  return reading_3 + reading_4 + reading_5;
}

float get_My(float reading_3, float reading_4, float reading_5) {
  return (reading_3 - reading_4)*LENGTH;
}

float get_Mx(float reading_3, float reading_4, float reading_5) {
  return (reading_5 - (reading_3 + reading_4)/2)*WIDTH_BOTTOM;
}

