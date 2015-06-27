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
// Tuning parameters
//
#define TUNE_KP (0.6) // (P)roportional term
#define TUNE_KI (0.1)  // (I)ntergral term
#define TUNE_KD (0.0)  // (D)erivative term
#define TUNE_KF (1000) // (F)requency of checks (ms)

//
// PWM output bounds
//
#define OUTPUT_MIN (0.0)
#define OUTPUT_MAX (255.0)

#endif
