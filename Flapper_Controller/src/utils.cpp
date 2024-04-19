#include <Math.h>

//amplitude in deg, frequency in Hz, phase in rad, time in ms, offset in deg, deadband_low in deg, deadband_high in deg (deadbands relative to offset, only pos values)
float sineWave(float amplitude, float frequency, float phase, float time, float offset = 0.0, float deadband_low = 0.0, float deadband_high = 0.0) {
    amplitude = amplitude- deadband_low - deadband_high;
    float value = amplitude * sin(2 * M_PI * frequency * (time/1000) + phase) + offset;
    if (value < offset) {
        value = value - deadband_low;
    } else if (value > offset) {
        value = value+ deadband_high;
    }
    return value;
}

//TODO low pass filter