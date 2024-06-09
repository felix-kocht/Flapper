#include <Arduino.h>
#include <Math.h>
#include "setpoints.h"

//Change these: 
static float frequency = 0.4; //Hz, ca. 0.875Hz per m/s
static float heave_amplitude =90; //speed (half the total possible angle)
static float pitch_amplitude = 70; //degree
static float camber_amplitude = 90; //degree
static float general_phase = 0; //in rad (needed for changing frequency)
static float pitch_phase = M_PI/2; //pitch leading heave, in rad
static float camber_phase = pitch_phase; //heave leading camber, in rad
//End of changeable parameters

//amplitude in deg, frequency in Hz, phase in rad, time in ms, offset in deg, deadband_low in deg, deadband_high in deg (deadbands relative to offset, only pos values)
static float generate_sine_wave(float time, float amplitude, float frequency, float phase, float offset = 0.0) {
    return amplitude * sin(2 * M_PI * frequency * (time/1000.0) - phase) + offset;
}

// static float gradient_descent(float start_value,float time,float new_frequency){
//     float result = 1;
//     while (result > -0.8){
//         start_value = start_value - 0.1;
//         result = generate_sine_wave(time, 1, new_frequency, start_value, 0); 
//     }
//     return start_value;
// }

void update_sine_waves(float (*params)[5], float (*targets), float time){
    for (int i = 0; i < 5; i++){
        targets[i] = generate_sine_wave(time, params[0][i], params[1][i], params[2][i], params[3][i]); 
    }
}

//TODO: work with deviations based on feedback control for thrust and turn_torque
void tune_parameters(float (*sine_params_ptr)[5]){
    //TODO: add functionality for different params for different maneuvers
    // Set parameters for Heave servo
    sine_params_ptr[0][0] = heave_amplitude; // Amplitude A
    sine_params_ptr[1][0] = frequency;  // Frequency f
    sine_params_ptr[2][0] = general_phase;    // Phase 
    sine_params_ptr[3][0] = 270/2+20; // Offset
  // Set parameters for Pitch Right servo
    sine_params_ptr[0][1] = pitch_amplitude;    // Amplitude A
    sine_params_ptr[1][1] = frequency; // Frequency f
    sine_params_ptr[2][1] = general_phase + pitch_phase;  // pitch_phase
    sine_params_ptr[3][1] = 180/2 - 10;    // Offset //TODO: dont hardcode offset
  // Set parameters for Pitch Left servo
    sine_params_ptr[0][2] = pitch_amplitude;    // Amplitude A
    sine_params_ptr[1][2] = frequency; // Frequency f
    sine_params_ptr[2][2] = general_phase + pitch_phase + M_PI;     // pitch_phase
    sine_params_ptr[3][2] = 180/2 - 10;    // Offset //TODO: dont hardcode offset

    // Set parameters for Camber Right servo
    sine_params_ptr[0][3] = camber_amplitude;    // Amplitude A
    sine_params_ptr[1][3] = frequency;  // Frequency f
    sine_params_ptr[2][3] = general_phase + camber_phase + M_PI;  // pitch_phase
    sine_params_ptr[3][3] = 180/2;    // Offset

    // Set parameters for Camber Left servo
    sine_params_ptr[0][4] = camber_amplitude;    // Amplitude A
    sine_params_ptr[1][4] = frequency;  // Frequency f
    sine_params_ptr[2][4] = general_phase + camber_phase;     // pitch_phase
    sine_params_ptr[3][4] = 180/2;    // Offset
}

//TODO: make it work, so that it always starts at the lower end of the amplitude
void changeFrequency(float new_frequency, int time){
    //static float shift = gradient_descent(0.0,time,new_frequency); // moving the sine wave so that it always starts at the lower end of its amplitude

    static float shift = -3*M_PI/2 + (time/1000)*2*M_PI*new_frequency; // moving the sine wave so that it always starts at the lower end of its amplitude

    general_phase =  shift; //(shift - 2*M_PI*floor(shift/(2*M_PI))); //removing full circles

    frequency = new_frequency;
}

//finds phase with the lowest value for a normal sine wave at the new frequency at the current time

