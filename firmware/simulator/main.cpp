#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "PID_Controller.h"
#include "PID_Simulator.h"

const double SP = 260.0;  // Set point (C)
const double KP = 1.0;    // Proportional tuning term
const double KI = 0.0;    // Integral tuning term
const double KD = 0.0;    // Derivative tuning term
const double KF = 200.0;  // Update frequency (s)

// Starting temperature
const double START_TEMP = 25.0;

// Run simulator for this many milliseconds
const usec RUNTIME = 1000 * 1000;

int main(int argc, char **argv) {
  PID* ctrl = NULL;
  PIDSim* sim = NULL;
  double pwm = 0.0;
  double temp = START_TEMP;

  ctrl = new PID();
  ctrl->setSP(260.0); // Set point (C)
  ctrl->setKp(50.0);  // (P)roportional tuning term
  ctrl->setKi(0.0);   // (I)ntegral tuning term
  ctrl->setKd(0.0);   // (D)erivative tuning term
  ctrl->setDt(200.0); // Update frequency
  ctrl->setInput(temp);

  sim = new PIDSim();
  sim->setLastTemp(START_TEMP);

  printf("time,pwm,temperature\n");
  for (usec t=0; t<RUNTIME; t+=KF) {
    temp = sim->calculate(pwm, t);
    ctrl->setInput(temp);
    if (ctrl->compute(t)) {
      pwm = ctrl->getOutput();
      printf("%lu,%f,%f\n", t, pwm, temp);
    }
    //printf("%lu,%d,%f\n", t, pwm, temp);
  }

  return EXIT_SUCCESS;
}
