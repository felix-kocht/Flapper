    #include <Arduino.h>
    #include <Servo.h>  // Include Servo library
    #include <Math.h>   // Include Math library
    // Servo objects
    Servo heave_servo;  // Create a Servo object for heave_servo
Servo pitch_servo_right; // Create a Servo object for pitch_servo
Servo pitch_servo_left;  // Create a Servo object for pitch_servo
// Trimming and limits (pitching)
int trim_right = -5;  // Right and left are not mirrored
int trim_left = 27;  // Left is slightly off
int pitch_limit = 60;  // Pitch goes from Y to X degree
// Heave parameters
    int heave_angle = 0;  // Initialize the starting angle of heave_servo
    int heave_limit = 150;  // Heave goes from 0 to X degree - 150 here corresponds to 180 degree on the servo
int heave_steps = 5;  // Initializes pitch steps in which the servo moves
// Pitch parameters
int pitch_angle_right = 0;  // Initialize the starting angle of pitch_servo_right
int pitch_angle_left = 0;  // Initialize the starting angle of pitch_servo_left
int initial_pitch_angle_right = 30;  // Initialize the starting angle of pitch_servo - the used servo does not accept 0 as starting point
int initial_pitch_angle_left = 30;  // Initialize the starting angle of pitch_servo - the used servo does not accept 0 as starting point
int pitch_steps = 0;  // Initializes pitch steps in which the servo moves
// Servo delay
int delay_servo = 50;  // Waits Xms for the servo to reach the position - we are doing 5 degree per 50ms, so 1 degree per 1ms on the heave and 2 degree per 50ms pitch
// Setup function
void setup() {
  // Calculate initial angles and steps
  initial_pitch_angle_right = initial_pitch_angle_right + pitch_limit;
  pitch_angle_right = initial_pitch_angle_right - trim_right - pitch_limit/2;  // Calculate pitch_angle
  pitch_angle_left = initial_pitch_angle_left + trim_left + pitch_limit/2;  // Calculate pitch_angle
  pitch_steps = 4;  // Calculate pitch steps
  // Attach servos and set initial angles
    heave_servo.attach(9);  // Attach heave_servo to pin 9
  pitch_servo_right.attach(8);  // Attach right pitch servo to pin 8
  pitch_servo_left.attach(7);  // Attach left pitch servo to pin 7
    heave_servo.write(heave_angle);  // Set the initial angle of heave_servo
  pitch_servo_right.write(pitch_angle_right);  // Set the initial angle of pitch_servo_right
  pitch_servo_left.write(pitch_angle_left);  // Set the initial angle of pitch_servo_left
 
 
//Appendix
 
}
// Main loop
void loop() {
  // Control loop for heave and pitch
  for (heave_angle = 0; heave_angle < heave_limit; heave_angle += heave_steps) {  // Move the servos in steps of heave_steps degree
    // Adjust the pitch angle based on the current heave angle
    if (heave_angle <= (heave_limit/2)){
      pitch_angle_right -= pitch_steps;
      pitch_angle_left += pitch_steps;
    }else{
      pitch_angle_right += pitch_steps;
      pitch_angle_left -= pitch_steps;
}
    // Write the new angles to the servos
    pitch_servo_right.write(pitch_angle_right);
    pitch_servo_left.write(pitch_angle_left);
    heave_servo.write(heave_angle);
    delay(delay_servo);  // Wait for the servo to reach the position
}
  // Control loop for heave and pitch
  for (heave_angle = heave_limit; heave_angle > 0; heave_angle -= heave_steps) {  // Move the servos in steps of heave_steps degree
    // Adjust the pitch angle based on the current heave angle
    if (heave_angle >= (heave_limit/2)){
      pitch_angle_right += pitch_steps;
      pitch_angle_left -= pitch_steps;
    }else{
      pitch_angle_right -= pitch_steps;
      pitch_angle_left += pitch_steps;
}
    // Write the new angles to the servos
    pitch_servo_right.write(pitch_angle_right);
    pitch_servo_left.write(pitch_angle_left);
    heave_servo.write(heave_angle);
    delay(delay_servo);  // Wait for the servo to reach the position
} }
