/*
 * PID_Controller.cpp
 *
 * More optimized PID controller for handling a heating unit without WHOA
 * overshoot (as the basic Arduino library seems to do).
 *
 */

#include "PID_Controller.h"
#include "usec.h"

PIDController::PIDController() {
  //lastTime = millis() - dt;
  lastInput = 0.0;
  lastDelta = 0.0;
}

PIDController::PIDController(
    //, double output,
    //double input,
    double sp,
    double newKp, double newKi, double newKd
    //bool reversed=false,
    //usec dt=100,
    //double newOutMin=0.0, double newOutMax=255.0)
)
{
  //setInput(input);
  //this->output = output;
  setSP(sp);
  setTuning(newKp, newKi, newKd);
  PIDController();
}

/**
 * Magic happens here. Formula used (velocity form, re-rewitten in
 * idealized/independent form):
 *
 *    d                 d                           d
 *   -- output += kp * -- e(t) + ki * e(t) - kd * ---- input
 *   dt                dt                         dt^2
 *
 * e(t) = set_point - input
 *
 * 1) This idealized form is converted to code by approximating both
 * derrivatives using data from the previous loop runs and the time interval
 * between.
 *
 * 2) The derrivative term is based on the change in input rather than the
 * error compared to set point. This avoids "derrivative kick" from a massive
 * change in set point, such as would happen on startup (where the lastInput is
 * always 0).
 *
 * 3) The velocity form is being used to avoid integration wind-up, which
 * causes the output to max out and significant error to accumulate over time.
 * This causes things to take way longer than they should to settle down.
 *
 * 4) Using the velocity form has drawbacks, namely it tends to amplify noise
 * in the signal rather than drown it out over time.
 *
 * 5) Using actual time difference rather than assuming it will be dt, since
 * I'm not going to assume perfect scheduling.
 *
 */
bool PIDController::compute(usec now) {
  usec diff = USEC_DIFF(now, lastTime);
  bool retval = false;

  if (diff >= dt) {
    double e = sp - input;
    double e_1 = sp - lastInput;
    double t = diff / 1000.0;
    double delta = input - lastInput;

    double dp = kp * (e - e_1);
    double di = ki * e * t;
    double dd = kd * (delta - lastDelta);

    double o = output + dp + di - dd;

    // Check for output saturation
    if (o < outputMin) {
      o = outputMin;
    } else if (o > outputMax) {
      o = outputMax;
    }
    output = o;

    // Save for next time
    lastTime = now;
    lastInput = input;
    lastDelta = delta;

    retval = true;
  }

  return retval;
}

double PIDController::getInput()     { return input; }
double PIDController::getOutput()    { return output; }
double PIDController::getSP()        { return sp; }
double PIDController::getKp()        { return kp; }
double PIDController::getKi()        { return ki; }
double PIDController::getKd()        { return kd; }
bool PIDController::isReversed()     { return reversed; }
usec PIDController::getDt()          { return dt; }
double PIDController::getOutputMin() { return outputMin; }
double PIDController::getOutputMax() { return outputMax; }

void PIDController::setInput(double newInput) {
  lastInput = input;
  input = newInput;
}

void PIDController::setOutput(double newOutput) {
  output = newOutput;
}

void PIDController::setSP(double newSP) {
  sp = newSP;
}

void PIDController::setKp(double newKp) {
  kp = newKp;
}

void PIDController::setKi(double newKi) {
  ki = newKi;
}

void PIDController::setKd(double newKd) {
  kd = newKd;
}

void PIDController::setReversed(bool newReversed) {
  reversed = newReversed;
}

void PIDController::setDt(usec newDt) {
  dt = newDt;
}

void PIDController::setOutputMin(double newOutputMin) {
  outputMin = newOutputMin;
}

void PIDController::setOutputMax(double newOutputMax) {
  outputMax = newOutputMax;
}

void PIDController::setTuning(double newKp, double newKi, double newKd) {
  kp = newKd;
  ki = newKi;
  kd = newKd;
}

void PIDController::setBounds(double newOutputMin, double newOutputMax) {
  outputMin = newOutputMin;
  outputMax = newOutputMax;
}

#if 0
void PIDController::serialDebugDump() {

  Serial.print("input="); Serial.print(input);
  Serial.print(" output="); Serial.println(output);

  Serial.print("kp="); Serial.print(kp);
  Serial.print(" ki="); Serial.print(ki);
  Serial.print(" kd="); Serial.println(kd);

  Serial.print("lastTime="); Serial.print(lastTime);
  Serial.print(" lastInput="); Serial.print(lastInput);
  Serial.print(" lastDelta="); Serial.println(lastDelta);

  while (!Serial.find("\r"))
    ;

}
#endif
