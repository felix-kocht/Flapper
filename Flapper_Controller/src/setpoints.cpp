#include <Arduino.h>
#include <Math.h>
#include "setpoints.h"

//Changeable parameters:
const float HEAVE_PHASE = M_PI/2; //do not change, otherwise changing frequency might not work well anymore
const float MAX_HEAVE_AMP = 40; //safety measure to avoid destruction
const float MAX_FREQ = 1.2; //safety measure to avoid destruction
const int HEAVE_OFFSET = 270/2-40; //offset for heave

static float frequency = 0.0; //Hz, ca. 0.875Hz per m/s
static float heave_amplitude = 40; //degree (half the total possible angle)
static float pitch_amplitude = 60; //degree
static float camber_amplitude = 60; //degree (tests showed more than 60 could lead to destruction)
static float pitch_phase = 0; //TODO define
static float camber_phase = 0; //TODO define
static float turn_factor = 0; //0 is straight, -2 is full left, 2 is full right, more is mayhem
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
    static float left_turn_factor = 1;
    static float right_turn_factor = 1;

    if (turn_factor > 0){//right turn
        left_turn_factor = 1;
        right_turn_factor = 1 - turn_factor;
    } else {//left turn
        left_turn_factor = 1 + turn_factor;
        right_turn_factor = 1;
    }

    // Set parameters for Heave servo
    sine_params_ptr[0][0] = heave_amplitude; // Amplitude A
    sine_params_ptr[1][0] = frequency;  // Frequency f
    sine_params_ptr[2][0] = HEAVE_PHASE;    // Phase 
    sine_params_ptr[3][0] = HEAVE_OFFSET;    // Offset
  // Set parameters for Pitch Right servo
    sine_params_ptr[0][1] = pitch_amplitude * right_turn_factor;    // Amplitude A
    sine_params_ptr[1][1] = frequency; // Frequency f
    sine_params_ptr[2][1] = pitch_phase;  // pitch_phase
    sine_params_ptr[3][1] = 180/2;    // Offset //TODO: dont hardcode offset
  // Set parameters for Pitch Left servo
    sine_params_ptr[0][2] = pitch_amplitude * left_turn_factor;    // Amplitude A
    sine_params_ptr[1][2] = frequency; // Frequency f
    sine_params_ptr[2][2] = pitch_phase + M_PI;     // pitch_phase
    sine_params_ptr[3][2] = 180/2;    // Offset //TODO: dont hardcode offset

    // Set parameters for Camber Right servo
    sine_params_ptr[0][3] = camber_amplitude * right_turn_factor;    // Amplitude A
    sine_params_ptr[1][3] = frequency;  // Frequency f
    sine_params_ptr[2][3] = camber_phase ;  // pitch_phase
    sine_params_ptr[3][3] = 180/2;    // Offset

    // Set parameters for Camber Left servo
    sine_params_ptr[0][4] = camber_amplitude * left_turn_factor;    // Amplitude A
    sine_params_ptr[1][4] = frequency;  // Frequency f
    sine_params_ptr[2][4] = camber_phase + M_PI;     // pitch_phase
    sine_params_ptr[3][4] = 180/2;    // Offset
}

//TODO: make it work, so that it always starts at the lower end of the amplitude
void change_frequency(float new_frequency){
    frequency = new_frequency;
    if (new_frequency > MAX_FREQ){ //limiting to max frequency
        frequency = MAX_FREQ;
    }
    
}

void change_rest(float heave_amplitude2, float pitch_amplitude2, float camber_amplitude2, float pitch_phase2, float camber_phase2, float turn_factor2){
    heave_amplitude = heave_amplitude2;
    if (heave_amplitude > MAX_HEAVE_AMP){ //limiting to max heave
        heave_amplitude = MAX_HEAVE_AMP;
    }
    pitch_amplitude = pitch_amplitude2;
    camber_amplitude = camber_amplitude2;
    pitch_phase = pitch_phase2;
    camber_phase = camber_phase2;
    turn_factor = turn_factor2;
}

float get_minimum_heave(){
    return HEAVE_OFFSET - heave_amplitude;
}

