#include <Math.h>
#include <assert.h>

// how are these defined exactly?, should i work with camber or servo angle?
float frequency = 0.4; //Hz, ca. 0.875Hz per m/s
float heave_amplitude =90; //speed (half the total possible angle)
float pitch_amplitude = 70; //degree
float camber_amplitude = 90; //degree
float general_phase = 0; //in rad (needed for changing frequency)
float pitch_phase = M_PI/2; //pitch leading heave, in rad
float camber_phase = pitch_phase; //heave leading camber, in rad


//amplitude in deg, frequency in Hz, phase in rad, time in ms, offset in deg, deadband_low in deg, deadband_high in deg (deadbands relative to offset, only pos values)
static float sineWave(float time, float amplitude, float frequency, float phase, float offset = 0.0) {
    return amplitude * sin(2 * M_PI * frequency * (time/1000.0) - phase) + offset;
}

static float calculatePhaseShift(float time, float new_freq, float value, float slope) {
    //Step 0: calculate entire passed phase from total start
    static float total_phase = time/1000.0 * 2 * M_PI * new_freq;

    //Step 1: Calculate phase from last sine restart to now
    float cycle_phase = asin(value);

    // Adjust based on the slope
    if (slope < 0) {
        cycle_phase = M_PI - cycle_phase;
    }

    //Step 2: Calculate how many full cycles have passed
    int full_cycles = floor((total_phase - cycle_phase) / (2 * M_PI)); // it is rounded down

    //Step 3: Calculate the remaining shift
    return total_phase - full_cycles * 2 * M_PI;
}

void changeFrequency(float new_frequency, float time){

    //extracting the old wall conditions:
    static float value = sineWave(time, 1, frequency, general_phase, 0);
    static float slope = sineWave(time + 10, 1, frequency, general_phase, 0)-value; //one ms later to get the slope direction

    //DEBUG
    value = -0.9;
    slope = 1;


    general_phase = calculatePhaseShift(time, new_frequency, value, slope); 
    frequency = new_frequency; //writing into the global variable
    
}

//TODO: work with deviations based on feedback control for thrust and turn_torque
void parameter_tuner(float waterspeed, float thrust, float target_thrust, float turn_torque, float target_turn_torque, float (*sine_params_ptr)[5]){
    //TODO: do actual calculatons
    // Set parameters for Heave servo
    sine_params_ptr[0][0] = heave_amplitude; // Amplitude A
    sine_params_ptr[0][1] = frequency;  // Frequency f
    sine_params_ptr[0][2] = general_phase;    // Phase 
    sine_params_ptr[0][3] = 270/2+20; // Offset
    sine_params_ptr[0][4] = 0;  // Deadband //TODO: if never needed, remove deadband functionality

    // Set parameters for Pitch Right servo
    sine_params_ptr[1][0] = pitch_amplitude;    // Amplitude A
    sine_params_ptr[1][1] = frequency; // Frequency f
    sine_params_ptr[1][2] = general_phase + pitch_phase;  // pitch_phase
    sine_params_ptr[1][3] = 180/2 - 10;    // Offset //TODO: dont hardcode offset
    sine_params_ptr[1][4] = 0;     // Deadband

    // Set parameters for Pitch Left servo
    sine_params_ptr[2][0] = pitch_amplitude;    // Amplitude A
    sine_params_ptr[2][1] = frequency; // Frequency f
    sine_params_ptr[2][2] = general_phase + pitch_phase + M_PI;     // pitch_phase
    sine_params_ptr[2][3] = 180/2 - 10;    // Offset //TODO: dont hardcode offset
    sine_params_ptr[2][4] = 0;     // Deadband

    // Set parameters for Camber Right servo
    sine_params_ptr[3][0] = camber_amplitude;    // Amplitude A
    sine_params_ptr[3][1] = frequency;  // Frequency f
    sine_params_ptr[3][2] = general_phase + camber_phase + M_PI;  // pitch_phase
    sine_params_ptr[3][3] = 180/2;    // Offset
    sine_params_ptr[3][4] = 0;     // Deadband

    // Set parameters for Camber Left servo
    sine_params_ptr[4][0] = camber_amplitude;    // Amplitude A
    sine_params_ptr[4][1] = frequency;  // Frequency f
    sine_params_ptr[4][2] = general_phase + camber_phase;     // pitch_phase
    sine_params_ptr[4][3] = 180/2;    // Offset
    sine_params_ptr[4][4] = 0;     // Deadband
}

void updateSineWaves(float (*params)[5], float (*targets), float time){
    for (int i = 0; i < 5; i++){
        targets[i] = sineWave(time, params[i][0], params[i][1], params[i][2], params[i][3]); 
    }
}

// //for the sensor fusion
// float alpha_vis = 0.6; //Lübeck observer gain
// float alpha_heave_encoder = 0.9; //Lübeck observer gain
// float model[5] = {0}; //for all 5 servos, model of the system
// int counter = 0;
// int heave_down_cycles = 0;
// int total_heave_down_cycles = 0; 
// int last_total_heave_down_cycles = 2; //TODO calibrate for first iteration
// float drift[5] = {1}; //drift of the system

// void estimate_servo_states(float (*estimates_ptr), float (*targets_ptr), bool heave_down){
//     //TODO: implement microstate estimate (from Apriltags) into control, if needed
//     // estimates_ptr[i] = targets_ptr[i]; //just a mock for now
//     if(estimates_ptr[0] > 270){
//         estimates_ptr[0] = 270;
//     }else if(estimates_ptr[0] < 0){
//         estimates_ptr[0] = 0;
//     }else if (heave_down){// new cycle starts, estimate will be sensor position, drift is recalculated
//         // assert(targets_ptr[0] < 100); //calibrate to warn in case of weird values, if triggered: "Suspicious, the heave signal was triggered while we should be far from the bottom."
//         heave_down_cycles = heave_down_cycles + 1;
//         total_heave_down_cycles = total_heave_down_cycles + 1;
//         if (heave_down_cycles >= last_total_heave_down_cycles/2){ // so that it will be at the bottom deadpoint of motion
//             heave_down_cycles = -999; //so that it will not be triggered again
//             for (int i = 0; i < 5; i++){
//                 estimates_ptr[i] = 270/2+heave_amplitude; //TODO only accurate for heave motion, not for the other servos
//                 drift[i] = estimates_ptr[i]-targets_ptr[i]; //calculate new drift: (current error from actual value / counter)
//             }
//             counter = 0;
//         }else{
//             for (int i = 0; i < 5; i++){
//                 estimates_ptr[i] = targets_ptr[i] + drift[i]; 
//                 drift[i] = drift[i] * 0.99; //drift is reduced by 1% per cycle
//             }
//         }
//     }else{
//         if (total_heave_down_cycles > 1){
//             last_total_heave_down_cycles = total_heave_down_cycles;
//         }
//         for (int i = 0; i < 5; i++){
//             estimates_ptr[i] = targets_ptr[i] + drift[i]; 
//             drift[i] = drift[i] * 0.99; //drift is reduced by 1% per cycle
            
//         }
//         heave_down_cycles = 0;
//         total_heave_down_cycles = 0;
//     }
//     counter = counter + 1;
    
// }
// //reset counter when heave_down is true

// //TODO low pass filter
