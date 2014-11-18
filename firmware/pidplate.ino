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
#include <Adafruit_MAX31855.h>
#include <LiquidCrystal.h>
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
LiquidCrystal lcd = LiquidCrystal(RS, RW, E, DB4, DB5, DB6, DB7);

// PID Control setup
double currTemp   = 0.0;               // input (process) var (PV)
double setTemp    = TEMP_SET;          // set point var (SP)
double ssrPWM     = 0.0;               // output (controlled) var (CV)
double lastPWM    = NAN;               // last value of CV to print changes

PID ctrl = PID(setTemp, TUNE_SLOW_P, TUNE_SLOW_I, TUNE_SLOW_D);

/***********************************************************************
 * "Helping"
 ***********************************************************************/

#if 0
char dig2ascii(uint8_t d) {
  switch (d) {
    case 0x0: return '0';
    case 0x1: return '1';
    case 0x2: return '2';
    case 0x3: return '3';
    case 0x4: return '4';
    case 0x5: return '5';
    case 0x6: return '6';
    case 0x7: return '7';
    case 0x8: return '8';
    case 0x9: return '9';
    case 0xA: return 'A';
    case 0xB: return 'B';
    case 0xC: return 'C';
    case 0xD: return 'D';
    case 0xE: return 'E';
    case 0xF: return 'F';
    default: return ' ';
  }
}
#endif

double readThermocouple() {
  double t = tc.readCelsius();

  // Only set the current temp if it's sane to prevent the PID controller from
  // going haywire if the TC fucks up
  if (t != NAN && t > -100 && t < 3000)
    return t;
  else
    return 0.0;
}

//
// Display current temp, setpoint temp, SSR level, and time since boot
//
#if 0
void doDisplay() {
  char buff[17];
  uint8_t dig0, dig1, dig2;

  // Current temp

  dig0 = ((int)(currTemp / 100)) % 10;
  dig1 = ((int)(currTemp / 10)) % 10;
  dig2 = ((int)currTemp) % 10;

  buff[0] = (dig0)         ? dig2ascii(dig0) : ' ';
  buff[1] = (dig0 || dig1) ? dig2ascii(dig1) : ' ';
  buff[2] = dig2ascii(dig2);
  buff[3] = 'C';
  buff[4] = ' ';

  // Setpoint temp

  dig0 = ((int)(setTemp / 100)) % 10;
  dig1 = ((int)(setTemp / 10)) % 10;
  dig2 = ((int)setTemp) % 10;

  buff[5] = (dig0)         ? dig2ascii(dig0) : ' ';
  buff[6] = (dig0 || dig1) ? dig2ascii(dig1) : ' ';
  buff[7] = dig2ascii(dig2);
  buff[8] = 'C';
  buff[9] = ' ';

  // SSR level
  dig0 = ((int)(ssrPWM / 100)) % 10;
  dig1 = ((int)(ssrPWM / 10)) % 10;
  dig2 = ((int)ssrPWM) % 10;

  buff[10] = (dig0)         ? dig2ascii(dig0) : ' ';
  buff[11] = (dig0 || dig1) ? dig2ascii(dig1) : ' ';
  buff[12] = dig2ascii(dig2);
  buff[13] = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(buff);

  // Display time since startup in HH:MM:SS format
  usec now = millis() / 1000;
  uint32_t h = (now / (60 * 60)) % 100;
  uint32_t m = (now / 60) % 60;
  uint32_t s = now % 60;
  buff[0] = dig2ascii(h / 10);
  buff[1] = dig2ascii(h % 10);
  buff[2] = ':';
  buff[3] = dig2ascii(m / 10);
  buff[4] = dig2ascii(m % 10);
  buff[5] = ':';
  buff[6] = dig2ascii(s / 10);
  buff[7] = dig2ascii(s % 10);
  buff[8] = 0;
  lcd.setCursor(0, 1);
  lcd.print(buff);

}
#endif

void displayTime() {

  // HH:MM:SS\0
  // 01234567 8

  usec now = millis() / 1000;
  int h = now / 60 / 60 % 100;
  int m = now / 60 % 60;
  int s = now % 60;

  char buff[9];
  snprintf(buff, sizeof(buff), "%02.2d:%02.2d:%02.2d", h, m, s);

  lcd.setCursor(0, 1);
  lcd.print(buff);
}

//
// Display current temp, setpoint temp, SSR level, and time since boot
//
void displayTemp() {
  char buff[17];
  size_t pos = 0;

  // 123C 456C  255.0
  // 0123456789111111
  //           012345

  // Current temp
  dtostrf(round(fmod(currTemp, 1000)), 3, 0, buff + pos);
  buff[3] = 'C';
  buff[4] = ' ';
  pos = 5;

  // Setpoint temp
  dtostrf(round(fmod(setTemp, 1000)), 3, 0, buff + pos);
  buff[8] = 'C';
  buff[9] = ' ';
  buff[10] = ' ';
  pos = 11;

  // SSR level
  //dtostrf(fmod(ssrPWM, 1000), 5, 1, buff + pos);
  dtostrf(ssrPWM, 5, 1, buff + pos);
  buff[16] = '\0';

  lcd.setCursor(0, 0);
  lcd.print(buff);

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
  lcd.begin(LCD_COLS, LCD_ROWS);

  // Setup PID algorithm: Peform two back to back readings to initialize the
  // PID controller because fuck it
  currTemp = readThermocouple();
  ctrl.setInput(currTemp);
  currTemp = readThermocouple();
  ctrl.setInput(currTemp);

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
    // FIRE ZE MISSILES
    lastPWM = ssrPWM;
    ssrPWM = ctrl.getOutput();
    analogWrite(SSR, ssrPWM);
  }

  // Update display
  lcd.clear();
  displayTemp();
  displayTime();

  //ctrl.serialDebugDump();

}
