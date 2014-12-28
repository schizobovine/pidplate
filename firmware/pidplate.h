#ifndef __PIDPLATE_H
#define __PIDPLATE_H

#include<Arduino.h>

const int THERMO_DO  = 8;
const int THERMO_CS  = 9;
const int THERMO_CLK = 10;
const int SSR = 5;
const int BL  = 6;
const int E   = 7;
const int DB4 = 8;
const int DB5 = 9;
const int DB6 = 10;
const int DB7 = 11;
const int RS  = 12;
const int RW  = 13;

#define LCD_COLS (16)
#define LCD_ROWS (2)

// If the differences between the setpoint temp and the current temp drops
// below this threshold, switch to more conservative (smaller delta) tuning
// parameters so we don't overshoot.
//#define PID_TUNE_THRESHOLD (25)

// If the temp has gone above SP by this much (represents potential error in
// measurement), just fucking turn the goddamn thing off.
//#define OMG_THRESHOLD (5)

//
// Aggressive/faster tuning parameters
//
#define TUNE_FAST_P (50.0)  // (P)roportional term
#define TUNE_FAST_I (1.0)  // (I)ntergral term
#define TUNE_FAST_D (2.0) // (D)erivative term
#define TUNE_FAST_F (200)  // (F)requency of checks (ms)

//
// Conservative/slower tuning parameters
//
#define TUNE_SLOW_P (10.0)  // (P)roportional term
#define TUNE_SLOW_I (0.1)  // (I)ntergral term
#define TUNE_SLOW_D (2.0)  // (D)erivative term
#define TUNE_SLOW_F (200)  // (F)requency of checks (ms)

//#define PID_OUTPUT_MIN (0.0)
//#define PID_OUTPUT_MAX (255.0)

#define SIMULATION 0

#endif
