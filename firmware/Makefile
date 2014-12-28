#ARDUINO_LIBS  := LiquidCrystal Adafruit_MAX31855 PID_Controller
#ARDUINO_LIBS  := Adafruit_MAX31855 PID_Controller
ARDUINO_LIBS  := Adafruit_MAX31855 PID_Controller Adafruit_GFX Adafruit_LED_Backpack Wire
BOARD_TAG     := uno
ARDUINO_PORT  := /dev/ttyUSB0
USER_LIB_PATH := $(realpath $(HOME)/sketchbook/libraries/)
#USER_LIB_PATH += ":$(realpath ./lib/PID_Controller/)"
CXXFLAGS        += -std=gnu++11


include /usr/share/arduino/Arduino.mk
