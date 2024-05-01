#include <Math.h>
#include <assert.h>

// how are these defined exactly?, should i work with camber or servo angle?
float frequency = 0.8; //ca. 0.875Hz per m/s
float heave_amplitude = 1200; //speed
float pitch_amplitude = 40; //degree
float camber_amplitude = 90; //degree
float phase = M_PI/2; //pitch leading heave, in rad
// float camber_phase = 0; //heave leading camber, in rad

//for the sensor fusion
float alpha_vis = 0.6; //Lübeck observer gain
float alpha_heave_encoder = 0.9; //Lübeck observer gain
float model[5] = {0}; //for all 5 servos, model of the system
int counter = 0;
int heave_down_cycles = 0;
int total_heave_down_cycles = 0; 
int last_total_heave_down_cycles = 20; //TODO calibrate for first iteration
float drift[5] = {0}; //drift of the system

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
        targets[0] = sineWave(params[i][0], params[i][1], params[i][2], params[i][3], params[i][4], params[i][4], time); //is this correct syntax with the star?
    }
}

void estimate_servo_states(float (*estimates_ptr), float (*targets_ptr), bool heave_down){
    //TODO: implement microstate estimate (from Apriltags) into control, if needed
        // estimates_ptr[i] = targets_ptr[i]; //just a mock for now
    if (heave_down){// new cycle starts, estimate will be bottom position, drift is recalculated
        assert(targets_ptr[0] > 1); //calibrate to warn in case of weird values, if triggered: "Suspicious, the heave signal was triggered while we should be far from the bottom."
        heave_down_cycles = heave_down_cycles + 1;
        total_heave_down_cycles = total_heave_down_cycles + 1;
        if (heave_down_cycles >= last_total_heave_down_cycles/2){ // so that it will be at the bottom deadpoint of motion
            heave_down_cycles = -999; //so that it will not be triggered again
            counter = 0;
            for (int i = 0; i < 5; i++){
                estimates_ptr[i] = 0; //TODO only accurate after proper calibration of middle_of_sensor
                drift[i] = (targets_ptr[i] + drift[i]*counter) / counter; //calculate new drift: (current error from zero / counter)
            }
        }   
        
    }else{
        if (total_heave_down_cycles > 1){
            last_total_heave_down_cycles = total_heave_down_cycles;
        }
        for (int i = 0; i < 5; i++){
            estimates_ptr[i] = targets_ptr[i] + drift[i]*counter; 
        }
        heave_down_cycles = 0;
        total_heave_down_cycles = 0;
    }
    counter = counter + 1;
}
//reset counter when heave_down is true

//TODO low pass filter
