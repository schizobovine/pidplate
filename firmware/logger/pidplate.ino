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

#include "usec.h"
#include "pidplate.h"

/***********************************************************************
 * Globals
 ***********************************************************************/

// THERMOCOUPLE AMP
Adafruit_MAX31855 tc(THERMO_CLK, THERMO_CS, THERMO_DO);

// DISPLAY
Adafruit_SSD1306 display(DISPLAY_RST);

// UART data speed
const int SERIAL_BAUD = 9600;

// Milliseconds since start of run
usec time_start = 0;

// Milliseconds since last serial logging
usec time_last_dump = 0;

// Last temperature reading
double last_temp = 0.0;

// SSR level
double ssrPWM = 0.0;

/***********************************************************************
 * "Halping"
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
    return last_temp; 
  }
}

//
// Display current temp, setpoint temp, SSR level, and time since boot
//
void refreshDisplay(usec now) {

  // Format strings, stored in program memory to avoid wasting precious
  // precious RAMz.
  PGM_P DISP_FMT = PSTR(
         "Temp %#5.1f\367C"
    "\n" "SetP     OFF"
    "\n" "PWM      OFF"
    "\n" "  %2.2u:%2.2u:%2.2u"
  );

  // String-ify display data in a C-ish style way since the Arduino libs don't
  // really give us any way to handle this well (which is annoying). Note that
  // special linker flags are required to bring in the full vprintf
  // implementation for floating point number. Check the Makefile for the
  // voodoo.
  char buff[21*4+1];
  int h = now / 1000 / 60 / 60 % 100;
  int m = now / 1000 / 60 % 60;
  int s = now / 1000 % 60;
  snprintf_P(buff, sizeof(buff), DISP_FMT, last_temp, h, m, s);
  buff[sizeof(buff)-1] = '\0';

  // Actually shart chars to display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(buff);
  display.display();

}

//
// Output current time, temp, and SSR level to serial port
//
void outputData(usec now) {
  Serial.print(now, DEC);
  Serial.print(", ");
  Serial.print(last_temp, DEC);
  Serial.print(", ");
  Serial.print(ssrPWM, DEC);
  Serial.print("\r\n");
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

  // Initialize serial port after waiting a sec
  Serial.begin(SERIAL_BAUD);
  while (!Serial)
    ;

  // Wait for someone to pick up the phone
  while (!Serial.find(TOKEN_START))
    ;

  // Begin data dump
  time_start = millis();
  time_last_dump = time_start - LOG_INTERVAL_MS;
  Serial.println();
  Serial.println("time_ms,temp_c,pwm");

}

uint8_t repeated_reading = 0;

/***********************************************************************
 * loop() - this is done all the time (like your mom)
 ***********************************************************************/

void loop() {
  
  // Get reading from thermocouple
  last_temp = readThermocouple();

  // Get ticks since startup
  uint32_t now = millis();

  // Figure out if SSR needs to be ON or OFF
  if (USEC_DIFF(now, time_start) <= RUN_TIME) {
    if (ssrPWM <= 0) {
      ssrPWM = RUN_LEVEL;
      analogWrite(SSR, ssrPWM);
    }
  } else {
    if (ssrPWM >= 0) {
      ssrPWM = 0;
      analogWrite(SSR, ssrPWM);
    }
  }

  // Update display
  refreshDisplay(now);

  // If it's time, dump serial data
  if (USEC_DIFF(now, time_last_dump) >= LOG_INTERVAL_MS) {
    outputData(now);
  }

}

// vi: syntax=arduino sw=2 ts=2
