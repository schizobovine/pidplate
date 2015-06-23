/*
 * PID_Simulator.cpp
 *
 * Used to test assumptions about PID controller tunings.
 *
 */

#include "PID_Simulator.h"
#include "usec.h"

PIDSimulator::PIDSimulator() {
  reset();
}

void PIDSimulator::reset() {
  qVested = 0.0;
  qStored = 0.0;
  lastTemp = 0.0;
  lastTime = 0;
}

double PIDSimulator::calculate(double powerLevel, usec now) {
  double dt = 0.0; // Delta(time)
  double dq = 0.0; // Delta(heat)

  // How long since last calculation?
  dt = USEC_DIFF(now, lastTime);
  dt = dt / 1000.0;

  // Estimate heat dumped in based on how much time has passed
  dq += powerLevel/255 * dt * power;

  // Estimate heat loss due to ambient cooling
  dq -= (lastTemp - ambient) * dt * qLoss;

  // Determine how much heat shows up this round; if it's higher than the
  // vesting rate, it's stored for later realization once output starts to
  // drop.
  qStored += dq;
  if (qStored >= vestRate) {
    qStored -= vestRate;
    qVested += vestRate;
  } else {
    qVested += qStored;
    qStored = 0.0;
  }

  // Finally, figure out new current temp based purely on stored heat and
  // ambient temp
  lastTemp = ambient + qVested / heatCap;
  lastTime = now;

  return lastTemp;

}

double PIDSimulator::getLastTemp() {
  return lastTemp;
}

double  PIDSimulator::setLastTemp(double temp) {
  lastTemp = temp;
  return lastTemp;
}

double PIDSimulator::getStoredHeat() {
  return qStored;
}

double PIDSimulator::getFinalTemp() {
  return ambient + (qVested + qStored) / heatCap;
}
