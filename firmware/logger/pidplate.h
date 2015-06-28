#ifndef __PIDPLATE_H
#define __PIDPLATE_H

//
// Pins used for thermocouple sensor
//
const int THERMO_DO  = 8;
const int THERMO_CS  = 9;
const int THERMO_CLK = 10;
const int SSR = 3;

//
// Various constants for the OLED display
//
const int DISPLAY_RST = A3;
const int DISPLAY_TEXTSIZE = 1;
const int DISPLAY_COLOR = WHITE;
const int DISPLAY_ADDR = SSD1306_I2C_ADDRESS;
const int DISPLAY_MODE = SSD1306_SWITCHCAPVCC;

//
// Milliseconds between log outputs
//
#include "usec.h"
const usec LOG_INTERVAL_MS = 1000;

//
// If the last REPEAT_COUNT temperature readings have been the same, consider
// temperature stabilized.
// 
const uint8_t REPEAT_COUNT = 10;

//
// Length of time (ms) to crank up the SSR and measure the effects.
//
const usec RUN_TIME = 32 * 1000;

//
// Duty cycle for SSR (0-255, with 255 being 100%)
//
const double RUN_LEVEL = 255.0;

//
// Start message to look for on serial connection
//
#define TOKEN_START "START"

#endif
