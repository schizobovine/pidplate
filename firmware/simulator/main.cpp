#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "PID_Controller.h"
#include "PID_Simulator.h"

const double SP = 260.0;  // Set point (C)
const double KP = 0.61;   // Proportional tuning term
const double KI = -0.1; // Integral tuning term
const double KD = 0.0;    // Derivative tuning term
const double KF = 200.0;  // Update frequency (s)

// Starting temperature
const double START_TEMP = 25.0;

// Run simulator for at most this many milliseconds
const usec RUNTIME = 1000 * 1000;

// If PWM is within this amount from last round, considier it the "same"
const double PWM_THRESHOLD = 0.01;

// If temperature is within this threshold consider it the "same"
const double TEMP_THRESHOLD = 0.01;

int main(int argc, char **argv) {
  PID* ctrl = NULL;
  PIDSim* sim = NULL;
  double pwm_curr = 0.0;
  double pwm_last = 0.0;
  double temp = START_TEMP;

  ctrl = new PID();
  ctrl->setSP(SP);
  ctrl->setKp(KP);
  ctrl->setKi(KI);
  ctrl->setKd(KD);
  ctrl->setDt(KF);
  ctrl->setInput(temp);

  sim = new PIDSim();
  sim->setLastTemp(START_TEMP);

  printf("time,pwm_curr,curr_temp,iterm,final_temp\n");
  for (usec t=0; t<RUNTIME; t+=KF) {

    // Calculate current temp based on controller's settings
    temp = sim->calculate(pwm_curr, t);

    // Feed simulation data into controller
    ctrl->setInput(temp);
    if (ctrl->compute(t)) {
      pwm_last = pwm_curr;
      pwm_curr = ctrl->getOutput();
    }

    printf("%lu,%f,%f,%f,%f,\n", t, pwm_curr, temp, ctrl->getIntegralTerm(), sim->getFinalTemp());

    // If we've reached a steady state (PWM hasn't changed) and simulation
    // predicts no more temperature change, this means stored heat is now
    // balanced by ambient loss, so we can exit.
    if (fabs(pwm_curr - pwm_last) < PWM_THRESHOLD && fabs(temp - sim->getFinalTemp()) < TEMP_THRESHOLD) {
      printf("Stead-state reached, exiting...\n");
      return EXIT_SUCCESS;
    }

  }

  return EXIT_SUCCESS;
}
