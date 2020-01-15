//Contains the declaration of the state variables for the control loop
#include <math.h>

#include "State.h"
#include "../Configuration.h"

#include "SAMD51/board.h"

//---- interrupt vars ----
volatile byte mode = 0;

 float r = 0;  //target angle
 float y = 0;  //current angle
 int32_t steps = 0;  //target step

 float velLimit = 900.0;
 float accLimit = 10000.0;
 
 float error = 0;  //error angle

 float u = 0;   // control effort

 float electric_angle = 0;


 bool dir = true;             // flag for dir setting
 //bool enabled = true;         // flag for enabled setting










// Low-pass filter
// actual angle
Filter filter_y((float)FPID/3.0, FPID);
// setup Omega Filter to 500 Hz
Filter omegaFilter(500.0, FPID);

AS5047D myAS5047D(steps_per_revolution, &Serial, PORTA, 15);
A4954 myA4954(steps_per_revolution, 18, 19, 21, 20, 2, 5);

PIDControler myPID(&y, &u, &r);

stepInterface mystepInterface(step_pin, dir_pin, ena_pin, steps_per_revolution, microstepping);
