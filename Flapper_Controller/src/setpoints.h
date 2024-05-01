#ifndef SETPOINTS_H
#define SETPOINTS_H

float sineWave(float amplitude, float frequency, float phase, float offset = 0.0, float deadband_low = 0.0, float deadband_high = 0.0, float time);

//just mock these thrust and turn_torque values, if on the lake (a bit of calibration and it should be fine)
void parameter_tuner(float waterspeed, float thrust, float target_thrust, float turn_torque, float target_turn_torque, float (*params)[5]);


#endif