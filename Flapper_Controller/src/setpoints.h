#ifndef SETPOINTS_H
#define SETPOINTS_H


void update_sine_waves(float (*params)[5], float (*targets), float time); 

void tune_parameters(float (*sine_params_ptr)[5]);

#endif