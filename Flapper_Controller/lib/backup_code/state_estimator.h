#ifndef STATE_ESTIMATOR_H
#define STATE_ESTIMATOR_H

//struct to hold position
struct Macrostate {
    float x;
    float y;
    float v;
    //add more variables if needed
};

//struct to hold servo angles
struct Microstate {
    float heave;
    float pitch_right;
    float pitch_left;
    float camber_right;
    float camber_left; //maybe even add velocities?
};

// Function to initialize a servo with specific parameters
Macrostate getBoatState(); //TODO add sensor parameters for each sensor

Microstate getServoAngles(); //TODO add sensor parameters for each sensor

#endif