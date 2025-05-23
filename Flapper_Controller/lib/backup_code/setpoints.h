#ifndef SETPOINTS_H
#define SETPOINTS_H

extern float general_phase;

void updateSineWaves(float (*params)[5], float (*targets), float time); 

//float sineWave( float time, float amplitude, float frequency, float phase, float offset = 0.0);

//just mock these thrust and turn_torque values, if on the lake (a bit of calibration and it should be fine)
void parameter_tuner(float waterspeed, float thrust, float target_thrust, float turn_torque, float target_turn_torque, float (*params)[5]);

void changeFrequency(float new_frequency, float time);

//void estimate_servo_states(float (*estimates_ptr), float (*targets_ptr), bool heave_down);


#endif