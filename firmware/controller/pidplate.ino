/*
 * pidplate.ino
 *
 * Test sketch to build my very own PID (Psomething Interval(?) Derivative)
 * controller using a fucking ridiculous 40A solid state relay. (Seriously,
 * this thing is fucking overkill like WHOA.)
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <Arduino.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MAX31855.h>

#include <PID_Controller.h>

#include "usec.h"
#include "pidplate.h"

/***********************************************************************
 * Constants & Globals
 ***********************************************************************/

// The eventual temperature we want to hit
const double TEMP_SET = 50.0;

// THERMOCOUPLE AMP
Adafruit_MAX31855 tc(THERMO_CLK, THERMO_CS, THERMO_DO);

// DISPLAY
Adafruit_SSD1306 display(DISPLAY_RST);

// PID Control setup
double currTemp = 0.0;      // input (process) var (PV)
double setTemp  = TEMP_SET; // set point var (SP)
double ssrPWM   = 0.0;      // output (controlled) var (CV)
double lastPWM  = NAN;      // last value of CV to print changes
PID ctrl = PID();

// Serial debugging setup
usec last_blat = 0;
usec BLAT_INT = 1;

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
void refreshDisplay() {

  // Format strings, stored in program memory to avoid wasting precious
  // precious RAMz.
  PGM_P FMT = PSTR(
         "Temp %#5.1f\367C"
    "\n" "SetP %#5.1f\367C"
    "\n" "PWM      %3.0f"
    "\n" "  %2.2u:%2.2u:%2.2u"
  );

  // String-ify display data in a C-ish style way since the Arduino libs don't
  // really give us any way to handle this well (which is annoying). Note that
  // special linker flags are required to bring in the full vprintf
  // implementation for floating point number. Check the Makefile for the
  // voodoo.
  char buff[21*4+1];
  uint32_t now = millis() / 1000;
  int h = now / 60 / 60 % 100;
  int m = now / 60 % 60;
  int s = now % 60;
  snprintf_P(buff, sizeof(buff), FMT,
    currTemp, setTemp, ssrPWM,
    h, m, s
  );
  buff[sizeof(buff)-1] = '\0';

  // Actually shart chars to thingy
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(buff);
  display.display();

  if (USEC_DIFF(now, last_blat) > BLAT_INT) {
    last_blat = now;
    Serial.println(buff);
    Serial.println();
  }

}

/***********************************************************************
 * setup() - this is done once
 ***********************************************************************/

void setup() {

  // Initialize SSR to default off
  pinMode(SSR, OUTPUT);
  analogWrite(SSR, 0);

  // Boot up display
  Wire.begin();
  display.begin(DISPLAY_MODE, DISPLAY_ADDR);
  display.clearDisplay();
  display.setTextColor(DISPLAY_COLOR);
  display.setTextSize(DISPLAY_TEXTSIZE);
  display.println("");
  display.println("      PID Plate");
  display.println("        Hello!");
  display.display();

  // Setup PID algorithm
  ctrl.setSP(setTemp);
  ctrl.setKp(TUNE_KP);
  ctrl.setKi(TUNE_KI);
  ctrl.setKd(TUNE_KD);
  ctrl.setDt(TUNE_KF);
  ctrl.setOutputMin(OUTPUT_MIN);
  ctrl.setOutputMax(OUTPUT_MAX);

  // Peform two back to back readings to initialize the PID controller, so we
  // don't have to deal with the huge derivative term that results from a
  // sudden change in temp.
  currTemp = readThermocouple();
  ctrl.setInput(currTemp);
  delay(1000);
  currTemp = readThermocouple();
  ctrl.setInput(currTemp);
  delay(1000);

  Serial.begin(9600);
  Serial.println("Hello!");

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
  refreshDisplay();

}
