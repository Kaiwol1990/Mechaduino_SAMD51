//Contains the declaration of the state variables for the control loop

#ifndef __OBEJCTS_H__
#define __OBEJCTS_H__
//#include <stdint.h>

#include <arduino.h>
//#include "../Configuration.h"

#include "core/settings.h"
#include "output/A4954.h"
#include "sensor/AS5047D.h"
#include "pid/PIDControler.h"
#include "modules/filters.h"
#include "SAMD51/stepInterface.h"
#include "SAMD51/samd51TC0.h"
#include "SAMD51/samd51TC4.h"
#include "SAMD51/samd51TC5.h"
#include "SAMD51/samd51TRNG.h"
#include "SAMD51/samd51ADCSM.h"
#include "modules/Cmd.h"

//---- interrupt vars ----
/*
extern volatile float r;       //target angle
extern volatile float y;       //current angle
extern volatile int32_t steps; //target step
extern volatile float u;       // control effort
extern volatile bool dir;      // flag for  dir setting

extern float velLimit;
extern float accLimit;
*/
extern AS5047D myAS5047D;

extern A4954 myA4954;

extern PIDControler myPID;

extern stepInterface mystepInterface;

extern userSettings mySettings;

extern SerialCommander myCommander;

extern samd51TC4 mysamd51TC4;

extern samd51TC5 mysamd51TC5;

extern samd51TC0 mysamd51TC0;

extern samd51ADCSM mysamd51ADCSM;

extern samd51TRNG mysamd51TRNG;
#endif