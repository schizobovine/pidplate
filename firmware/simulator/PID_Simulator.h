#ifndef __PID_SIMULATOR_H
#define __PID_SIMULATOR_H

#include <stdint.h>
#include "usec.h"

// Heat "realized" (i.e., made visible to temperature probe) per second (J/s).
// This allows a rough approximation of the heat transfer between the coil and
// the plate.
const double VESTING_RATE = 50.0;

// Specific heat capacity. Iron is ~450J/(kg*K) and assuming ~1kg in plate to
// make math easier.
const double SPECIFIC_HEAT = 450.0;

// Power output of hotplate (W)
const double PLATE_POWER = 1000.0;

// Ambient temp for estimating loss to air w/Newton's Law of Cooling (C)
const double AMBIENT_TEMP = 25.0;

// Approximate heat loss rate to air (guesstimate of thermal conductivity)
const double AMBIENT_LOSS = 0.3;

class PIDSimulator {

  public:

    PIDSimulator();
    void reset();
    double calculate(double powerLevel, usec now);
    double getLastTemp();
    double setLastTemp(double temp);
    double getStoredHeat();
    double getFinalTemp();

  private:

    // Internal simulator variables
    double qVested  = 0.0; // Total heat "visible" (J)
    double qStored  = 0.0; // Total heat not yet "visible" (J)
    double lastTemp = 0.0; // Last temperature measurement (C)
    usec   lastTime =   0; // Epoch time of last temperature measurement (s)

    // Simulator tunings
    double vestRate = VESTING_RATE;
    double heatCap  = SPECIFIC_HEAT;
    double power    = PLATE_POWER;
    double ambient  = AMBIENT_TEMP;
    double qLoss    = AMBIENT_LOSS;

};

typedef class PIDSimulator PIDSim;

#endif
