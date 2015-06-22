#ifndef __PID_CONTROLLER_H
#define __PID_CONTROLLER_H

#include "usec.h"

class PIDController {

  public:

    /**
     * Constructor.
     *
     * @param input Input variable (aka controlled or process variable (CV/PV)
     * @param output Output variable (aka manipulated variable (MV)
     * @param setpt Set point, the value of the input variable we're trying to
     *        hit
     * @param kp Proportional tuning constant
     * @param ki Integral tuning constant
     * @param kd Derrivative tuning constant
     * @param reversed If true, increasing the output varible DECREASES the
     *        input variable (say, if you're an air conditioner).
     * @param dt How often we should check and re-evaluate input. THIS VARIABLE
     *        IS WAY MORE IMPORTANT THAN YOU REALIZE. It's the basis for how to
     *        handle the integral and derrivative terms. Thus, a very short
     *        value for a lagging control setup will have hilariously bad
     *        convergence time.
     * @param outputMin Minimum value for output (to control windup)
     * @param outputMax Maximum value for output (to control windup)
     *
     */
    PIDController(double sp, double kp, double ki, double kd);
    PIDController();

    /**
     * Computes new output value based on current conditions
     *
     * @returns True if the output value has changed
     */
    bool compute();

    //
    // Manipulate parameters
    //
    void setInput(double newInput);
    // NO SET OUTPUT
    void setSP(double newSP);
    void setKp(double newKp);
    void setKi(double newKi);
    void setKd(double newKd);
    void setReversed(bool newReversed);
    void setDt(usec newDt);
    void setOutputMin(double newOutputMin);
    void setOutputMax(double newOutputMax);

    //
    // Frobulate the tunings
    //
    void setTuning(double newKp, double newKi, double newKd);
    void setBounds(double newOutputMin, double newOutputMax);

    double getInput();
    double getOutput();
    double getSP();
    double getKp();
    double getKi();
    double getKd();
    bool isReversed();
    usec getDt();
    double getOutputMin();
    double getOutputMax();
    void serialDebugDump();

  private:
    void setOutput(double newOutput);
    double input = 0.0;
    double output = 0.0;
    double sp = 0.0;
    double kp = 0.0;
    double ki = 0.0;
    double kd = 0.0;
    bool reversed = false;
    usec dt = 100L;

    // We need to know these bounds as we'll liable to overshoot based on the
    // tuning paramters; if we overshoot, we need to properly account for that
    // rather than just capping the term (since that still causes ridiculous
    // overshoot).
    double outputMin = 0.0;
    double outputMax = 255.0;

    // From the last time calcuate was called
    usec lastTime = 0L;

    // Last input value so we can calculate error and approximate its
    // derrivative
    double lastInput = 0.0;

    // Rate of input change from last calculation for second derrivative
    // approximation (so we can use the velocity form of PID)
    double lastDelta = 0.0;
};

typedef class PIDController PID;

#endif
