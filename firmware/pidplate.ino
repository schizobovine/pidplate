/*
 * pidplate.ino
 *
 * Test sketch to build my very own PID (Psomething Interval(?) Derivative)
 * controller using a fucking ridiculous 40A solid state relay. (Seriously,
 * this thing is fucking overkill like WHOA.)
 *
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_MAX31855.h>
#include <PID_Controller.h>
#include "usec.h"
#include "pidplate.h"

/***********************************************************************
 * Constants & Globals
 ***********************************************************************/

// The eventual temperature we want to hit
//const double TEMP_SET = 260.0;
const double TEMP_SET = 260.0;

// Maximum increase in set point temp per second. Based on the heater being
// around 1kg iron, which has a specific heat capacity of 450J/kg*K. Since
// we've got 1000W of heating element, that gives:
//
// 450J/K / 1000J/s = 0.45K/s ~> 2.2s/K
//
// (Since Kelvin is just degrees C with an offset.)
//const double MAX_DELTA_SP = 2.2;

// THERMOCOUPLE AMP
Adafruit_MAX31855 tc = Adafruit_MAX31855(THERMO_CLK, THERMO_CS, THERMO_DO);

// DISPLAY
Adafruit_7segment display = Adafruit_7segment();

// PID Control setup
double currTemp = 0.0;      // input (process) var (PV)
double setTemp  = TEMP_SET; // set point var (SP)
double ssrPWM   = 0.0;      // output (controlled) var (CV)
double lastPWM  = NAN;      // last value of CV to print changes

PID ctrl = PID(setTemp, TUNE_SLOW_P, TUNE_SLOW_I, TUNE_SLOW_D);

/***********************************************************************
 * "Helping"
 ***********************************************************************/

double readThermocouple() {
  double t = tc.readCelsius();

  // Only set the current temp if it's sane to prevent the PID controller from
  // going haywire if the TC fucks up
  if (t != NAN && t > -100 && t < 3000) {
    return t;
  } else {
    //Return last reading so shit doesn't bounce around like a Jack Russell
    //fucking terrier
    return currTemp; 
  }
}

//
// Display current temp, setpoint temp, SSR level, and time since boot
//
void displayTemp() {
  display.print(currTemp);
  display.writeDisplay();
}

/***********************************************************************
 * setup() - this is done once
 ***********************************************************************/

void setup() {

  // Initialize SSR to default off
  pinMode(SSR, OUTPUT);
  analogWrite(SSR, 0);

  // Serial debugging
  delay(200);
  Serial.begin(9600);
  Serial.println("BALLS");
  delay(200);

  // Boot up display
  display.begin(0x70);

  // Setup PID algorithm: Peform two back to back readings to initialize the
  // PID controller because fuck it
  currTemp = readThermocouple();
  ctrl.setInput(currTemp);
  currTemp = readThermocouple();
  ctrl.setInput(currTemp);
  displayTemp();

}

/***********************************************************************
 * loop() - this is done all the time (like your mom)
 ***********************************************************************/

void loop() {
  
  // Get reading from thermocouple
  currTemp = readThermocouple();

  // Input temp to PID controller & calculate current PWM level
  ctrl.setInput(currTemp);
  if (ctrl.compute()) {
    lastPWM = ssrPWM;
    ssrPWM = ctrl.getOutput();
    analogWrite(SSR, ssrPWM);
  }

  // Update display
  display.print(currTemp);
  display.writeDisplay();

  //ctrl.serialDebugDump();

}
