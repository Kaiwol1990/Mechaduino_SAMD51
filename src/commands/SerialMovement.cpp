
#include "commands/SerialMovement.h"

#include "core/objects.h"
#include "core/variables.h"
#include "modules/Cmd.h"

#include "language/en.h"

void init_movement_menu()
{
  // generates the commands and dependencies for this "submenu"
  Commander.cmdAdd("motion", "set speed and acceleration", set_motion);
  Commander.cmdAdd(step_response_command, "execute a step response", step_response);
  Commander.cmdAdd("spline", "start spline movement", splineMovement);
  Commander.cmdAdd("pos", "print current position", reportPostition);
}

void reportPostition()
{
  if (Commander.check_argument(help_subcmd))
  {
    Serial.println("Prints the current position and target on the console");
    Serial.println(" ");
    return;
  }
  Serial.print("Target = ");
  Serial.print(r);
  Serial.print(", ");
  Serial.print("Postion = ");
  Serial.println(y);
}

void splineMovement()
{
  if (Commander.check_argument(help_subcmd))
  {
    Serial.println("Menu to execute spline movement with optional velocity and acceleration");
    Serial.println(" ");
    Serial.println(" spline -r [x] -v [x] -a [x]");
    Serial.println(" ");
    Serial.println("-r [x]  target angle in deg");
    Serial.println("-v [x]  set the maximal /  target velocity to the given value in deg/s");
    Serial.println("-a [x]  set the maximal /  target accleration to the given value in deg/s^2");
    Serial.println(" ");
    return;
  }

  // check for given velocity
  velLimit = Commander.return_float_argument("-v", velLimit, 100, 5000);
  accLimit = Commander.return_float_argument("-a", accLimit, 100, 50000);

  // calculate Target
  steps = ((Commander.return_float_argument("-r", r, -100000.0, 100000.0) * Settings.currentSettings.steps_per_Revolution * Settings.currentSettings.microstepping) / 360.0) + 0.5;
  stepInterface.writesteps(steps);
}

void set_motion()
{
  //if (Commander.check_argument(args, arg_cnt, help_subcmd))
  if (Commander.check_argument(help_subcmd))
  {
    Serial.println("Menu to maximal velocity and acceleration");
    Serial.println(" ");
    Serial.println(" motion -v [x] -a [x]");
    Serial.println(" ");
    Serial.println("-v [x]  set the maximal /  target velocity to the given value in deg/s");
    Serial.println("-a [x]  set the maximal /  target accleration to the given value in deg/s^2");
    Serial.println("-m [x]  set the motion planing mode");
    Serial.println(" ");
    return;
  }

  float velLimit_temp = Commander.return_float_argument("-v", velLimit, 1, 1000000);
  float accLimit_temp = Commander.return_float_argument("-acc", accLimit, 1, 1000000);
  uint8_t mode = Commander.return_float_argument("-m", Planner.getMode(), 0, 2);
  velLimit = velLimit_temp;
  accLimit = accLimit_temp;

  if (mode != Planner.getMode())
  {
    if (PID.getState())
    {
      Serial.println("Can't change motion planing while PID loop ist active!");
    }
    else
    {
      Serial.println("Changing motion planningmode");
      Planner.setMode(mode);
    }
  }
}

void step_response()
{
  //if (Commander.check_argument(args, arg_cnt, help_subcmd))
  if (Commander.check_argument(help_subcmd))
  {
    Serial.println("Menu execute a step response");
    Serial.println(" ");
    Serial.println(" response -r [x] -v [x] -a [x]");
    Serial.println(" ");
    Serial.println("-r [x]  motion length in deg");
    Serial.println("-v [x]  set the maximal /  target velocity to the given value in deg/s");
    Serial.println("-a [x]  set the maximal /  target accleration to the given value in deg/s^2");
    Serial.println(" ");
    return;
  }

  Serial.println(step_response_header);

  int response_steps = ((Commander.return_float_argument("-r", r, -1000.0, 1000.0) * Settings.currentSettings.steps_per_Revolution * Settings.currentSettings.microstepping) / 360.0) + 0.5;

  // check for given velocity
  velLimit = Commander.return_float_argument("-v", velLimit, 100, 5000);
  accLimit = Commander.return_float_argument("-a", accLimit, 100, 50000);

  int frequency = 5000;

  Serial.print("Steps = ");
  Serial.println(response_steps);
  Serial.print("Frequency = ");
  Serial.println(frequency);

  bool last_dir = dir;

  float answer[2000];
  float target[2000];
  float effort[2000];

  int counter = 0;

  unsigned int current_time = micros();
  unsigned int next_time = current_time;

  float dt = 1000000.0 / (float)frequency;

  // calculate the target vector
  target[0] = stepInterface.readsteps();
  for (int i = 1; i < 2000; i++)
  {
    target[i] = target[i - 1];

    if (i == 200)
    {
      target[i] = target[i - 1] + response_steps;
    }
  }

  dir = true;
  //enabled = true;
  PID.enable();
  stepInterface.writesteps(target[0]);
  delay(2000);
  while (counter < 2000)
  {
    current_time = micros();

    if (current_time >= next_time)
    {
      next_time = current_time + dt;

      answer[counter] = AS5047D.getAngle();
      effort[counter] = u;
      stepInterface.writesteps(target[counter]);
      target[counter] = r;
      //steps = target[counter];

      counter += 1;
    }
  }

  Serial.println("time;target;anlge;effort");
  for (int i = 0; i < 2000; i++)
  {
    Serial.print((float)((float)i * (float)dt / 1000000.0), 5);
    Serial.print(';');
    Serial.print(target[i], 5);
    Serial.print(';');
    Serial.print(answer[i], 5);
    Serial.print(';');
    Serial.println(effort[i], 5);
  }

  // set parameters back to the values before the
  //enabled = last_enabled;
  PID.disable();
  dir = last_dir;
}