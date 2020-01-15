//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:

#include "Configuration.h"

#include "core/Controler.h"
#include "core/State.h"
#include "core/Utils.h"
#include "core/Serial.h"
#include "core/Planner.h"

#include "modules/custommath.h"
#include "SAMD51/wiringMechaduino.h"

#define MaxError 0.3


//---- Step settings -----
const float stepangle = (360.0 / (Init_steps_per_revolution * Init_microstepping));   // angle of one microstep as float
const float PA = 360.0 / Init_steps_per_revolution;                      // angle of one fullstep as int

// ----- gets called with FPID -----
// ----- calculates the target velocity and PID settings -----
void ControlerLoop() {


  static float r_1;

  float omega;



  //----- Calculations -----

    // read the current angle
    myAS5047D.readAngle();
    y = myAS5047D.getAngle();


    // read step register
    mystepInterface.readsteps();
    steps = mystepInterface.getsteps();

    // Motion planing
    r = trapezodialSpline(stepangle * (float)steps);


    // omega target calculation
    omega = (r - r_1) * FPID;



    // -------------------------------------
    // -           PID loop           -
    // -------------------------------------
    myPID.compute();




    // Reset if not enabled
    if (myPID.getState()) {
      digitalFASTWrite(PORTA, 9, HIGH);
    }
    else {
      digitalFASTWrite(PORTA, 9, LOW);
      mystepInterface.writeangle(y);
    }

   
    // try to predict phase lag
    float phaseAdvanced = (omega * 0.0001163);
    
    // write output
    myA4954.output(y + phaseAdvanced, u);



    // Motor Voltage and Coil Temp check
    //AnalogReadStateMachine();

    r_1 = r;


}