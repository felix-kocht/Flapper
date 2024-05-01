#ifndef UTILS_H
#define UTILS_H

float sineWave(float amplitude, float frequency, float phase, float offset = 0.0, float deadband_low = 0.0, float deadband_high = 0.0, float time);

void parameter_tuner_bench(float thrust, float target_thrust, float turn_torque, float target_turn_torque, float (*params)[5]);

/* void parameter_tuner_boat(float speed, float target_speed, float turn_rate, float target_turn_rate, float *params); */

#endif