#include <Math.h>

// how are these defined exactly?, should i work with camber or servo angle?
float frequency = 0.8; //ca. 0.875Hz per m/s
float heave_amplitude = 1200; //speed
float pitch_amplitude = 40; //degree
float camber_amplitude = 90; //degree
float phase = M_PI/2; //pitch leading heave, in rad
// float camber_phase = 0; //heave leading camber, in rad

//amplitude in deg, frequency in Hz, phase in rad, time in ms, offset in deg, deadband_low in deg, deadband_high in deg (deadbands relative to offset, only pos values)
float sineWave(float amplitude, float frequency, float phase, float offset = 0.0, float deadband_low = 0.0, float deadband_high = 0.0, float time) {
    amplitude = amplitude- deadband_low - deadband_high;
    float value = amplitude * sin(2 * M_PI * frequency * (time/1000) - phase) + offset;
    if (value < offset) {
        value = value - deadband_low;
    } else if (value > offset) {
        value = value+ deadband_high;
    }
    return value;
}

//TODO: work with deviations based on feedback control for thrust and turn_torque
void parameter_tuner(float waterspeed, float thrust, float target_thrust, float turn_torque, float target_turn_torque, float (*sine_params_ptr)[5]){
    //TODO: do actual calculatons
    // Set parameters for Heave servo
    sine_params_ptr[0][0] = heave_amplitude; // Amplitude A
    sine_params_ptr[0][1] = frequency;  // Frequency f
    sine_params_ptr[0][2] = phase-M_PI/2;    // Phase (looks weird because heave servo is continous --> speed signal)
    sine_params_ptr[0][3] = 1500; // Offset
    sine_params_ptr[0][4] = 150;  // Deadband

    // Set parameters for Pitch Right servo
    sine_params_ptr[1][0] = pitch_amplitude;    // Amplitude A
    sine_params_ptr[1][1] = frequency; // Frequency f
    sine_params_ptr[1][2] = M_PI;  // Phase
    sine_params_ptr[1][3] = 90;    // Offset
    sine_params_ptr[1][4] = 0;     // Deadband

    // Set parameters for Pitch Left servo
    sine_params_ptr[2][0] = pitch_amplitude;    // Amplitude A
    sine_params_ptr[2][1] = frequency; // Frequency f
    sine_params_ptr[2][2] = 0;     // Phase
    sine_params_ptr[2][3] = 90;    // Offset
    sine_params_ptr[2][4] = 0;     // Deadband

    // Set parameters for Camber Right servo
    sine_params_ptr[3][0] = camber_amplitude;    // Amplitude A
    sine_params_ptr[3][1] = frequency;  // Frequency f
    sine_params_ptr[3][2] = M_PI;  // Phase
    sine_params_ptr[3][3] = 90;    // Offset
    sine_params_ptr[3][4] = 0;     // Deadband

    // Set parameters for Camber Left servo
    sine_params_ptr[4][0] = camber_amplitude;    // Amplitude A
    sine_params_ptr[4][1] = frequency;  // Frequency f
    sine_params_ptr[4][2] = 0;     // Phase
    sine_params_ptr[4][3] = 90;    // Offset
    sine_params_ptr[4][4] = 0;     // Deadband
}

void updateSineWaves(float (*params)[5], float (*targets), float time){
    for (int i = 0; i < 5; i++){
        *targets[0] = sineWave(params[i][0], params[i][1], params[i][2], params[i][3], params[i][4], params[i][4], time); //is this correct syntax with the star?
    }
}

//TODO low pass filter
