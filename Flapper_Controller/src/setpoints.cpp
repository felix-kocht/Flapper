#include <Arduino.h>
#include <Math.h>
#include "setpoints.h"

//Changeable parameters:
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
    sine_params_ptr[3][1] = 180/2;    // Offset //TODO: dont hardcode offset
  // Set parameters for Pitch Left servo
    sine_params_ptr[0][2] = pitch_amplitude;    // Amplitude A
    sine_params_ptr[1][2] = frequency; // Frequency f
    sine_params_ptr[2][2] = general_phase + pitch_phase + M_PI;     // pitch_phase
    sine_params_ptr[3][2] = 180/2;    // Offset //TODO: dont hardcode offset

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
    frequency = new_frequency;
}


