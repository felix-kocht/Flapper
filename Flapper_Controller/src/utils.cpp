#include <Math.h>

//amplitude in deg, frequency in Hz, phase in rad, time in ms, offset in deg, deadband_low in deg, deadband_high in deg (deadbands relative to offset, only pos values)
float sineWave(float amplitude, float frequency, float phase, float offset = 0.0, float deadband_low = 0.0, float deadband_high = 0.0, float time) {
    amplitude = amplitude- deadband_low - deadband_high;
    float value = amplitude * sin(2 * M_PI * frequency * (time/1000) + phase) + offset;
    if (value < offset) {
        value = value - deadband_low;
    } else if (value > offset) {
        value = value+ deadband_high;
    }
    return value;
}
void parameter_tuner_bench(float thrust, float target_thrust, float turn_torque, float target_turn_torque, float (*sine_params_ptr)[5]){
    //TODO: do actual calculatons
    // Set parameters for Heave servo
    sine_params_ptr[0][0] = 1200; // Amplitude A
    sine_params_ptr[0][1] = 0.8;  // Frequency f
    sine_params_ptr[0][2] = 0;    // Phase
    sine_params_ptr[0][3] = 1500; // Offset
    sine_params_ptr[0][4] = 450;  // Deadband

    // Set parameters for Pitch Right servo
    sine_params_ptr[1][0] = 40;    // Amplitude A
    sine_params_ptr[1][1] = 10 * 0.13; // Frequency f
    sine_params_ptr[1][2] = M_PI;  // Phase
    sine_params_ptr[1][3] = 90;    // Offset
    sine_params_ptr[1][4] = 0;     // Deadband

    // Set parameters for Pitch Left servo
    sine_params_ptr[2][0] = 40;    // Amplitude A
    sine_params_ptr[2][1] = 10 * 0.13; // Frequency f
    sine_params_ptr[2][2] = 0;     // Phase
    sine_params_ptr[2][3] = 90;    // Offset
    sine_params_ptr[2][4] = 0;     // Deadband

    // Set parameters for Camber Right servo
    sine_params_ptr[3][0] = 90;    // Amplitude A
    sine_params_ptr[3][1] = 0.13;  // Frequency f
    sine_params_ptr[3][2] = M_PI;  // Phase
    sine_params_ptr[3][3] = 90;    // Offset
    sine_params_ptr[3][4] = 0;     // Deadband

    // Set parameters for Camber Left servo
    sine_params_ptr[4][0] = 90;    // Amplitude A
    sine_params_ptr[4][1] = 0.13;  // Frequency f
    sine_params_ptr[4][2] = 0;     // Phase
    sine_params_ptr[4][3] = 90;    // Offset
    sine_params_ptr[4][4] = 0;     // Deadband
}

//TODO low pass filter