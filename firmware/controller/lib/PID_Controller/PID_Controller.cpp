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
  iTerm = 0.0;
  lastInput = 0.0;
  //lastDelta = 0.0;
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
 * Magic happens here. Formula used, idealized form since I grok it better:
 *
 *                                                      d
 * output = Kp * e(t) + Ki * Integral(e(t), dt) + Kd * -- e(t)
 *                                                     dt
 *
 * 1) This idealized form is converted to code by approximating both the
 * integral and derrivative terms using data from the previous loop runs and
 * the time interval between.
 *
 * 2) The derrivative term is based on the change in input rather than the
 * error compared to set point. This avoids "derrivative kick" from a massive
 * change in set point, such as would happen on startup (where the lastInput is
 * always 0).
 *
 * 3) Using actual time difference rather than assuming it will be dt, since
 * I'm not going to assume perfect scheduling.
 *
 */
bool PIDController::compute(usec now) {
  usec diff = USEC_DIFF(now, lastTime);
  bool retval = false;

  if (diff >= dt) {

    // Pre-calculate some terms
    double e = sp - input;
    double t = diff / 1000.0;
    double delta = input - lastInput;

    // Add error to accumulator/integrator, factoring in actual time difference
    // by using ratio of the time passed and the nominal sample period.
    iTerm += ki * e * (t / dt);
    
    // Bound integrator to prevent significant error accumulation (i.e., no
    // more than could be fixed by a single cycle going to 0.
    if (iTerm > outputMax) {
      iTerm = outputMax;
    } else if (iTerm < outputMin) {
      iTerm = outputMin;
    }
    
    // Calculate new output
    double o = (kp * e) + iTerm + (kd * delta);

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
    //lastDelta = delta;

    retval = true;
  }

  return retval;
}

double PIDController::getInput()         { return input; }
double PIDController::getOutput()        { return output; }
double PIDController::getSP()            { return sp; }
double PIDController::getKp()            { return kp; }
double PIDController::getKi()            { return ki; }
double PIDController::getKd()            { return kd; }
bool PIDController::isReversed()         { return reversed; }
usec PIDController::getDt()              { return dt; }
double PIDController::getOutputMin()     { return outputMin; }
double PIDController::getOutputMax()     { return outputMax; }
double PIDController::getIntegralTerm()  { return iTerm; }

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
