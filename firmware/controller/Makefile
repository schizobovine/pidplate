ARDUINO_LIBS  := Adafruit_MAX31855 Adafruit_GFX Adafruit_SSD1306 SPI Wire PID_Controller
BOARD_TAG     := uno
ARDUINO_PORT  := /dev/ttyUSB0
USER_LIB_PATH := $(realpath $(HOME)/sketchbook/libraries/)
CXXFLAGS      += -std=gnu++11 -Wl,-u,vfprintf
CFLAGS        += -std=gnu++11 -Wl,-u,vfprintf
LDFLAGS       += -lprintf_flt -lm -Wl,-u,vfprintf


include /usr/share/arduino/Arduino.mk
