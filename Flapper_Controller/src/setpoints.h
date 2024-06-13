#ifndef SETPOINTS_H
#define SETPOINTS_H

void update_sine_waves(float (*params)[5], float (*targets), float time); 

void tune_parameters(float (*sine_params_ptr)[5]);

void change_frequency(float new_frequency);

float get_minimum_heave();

void change_rest(float heave_amplitude2, float pitch_amplitude2, float camber_amplitude2, float pitch_phase2, float camber_phase2, float turn_factor2);

#endif