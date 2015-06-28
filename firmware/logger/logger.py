#!/usr/bin/env python

from datetime import datetime
import serial
import sys

SERIAL_BAUD = 9600
SERIAL_PORT = '/dev/ttyUSB0'
OUTPUT_FILE = datetime.now().strftime('data/%Y%m%d_%H%M%s_data.csv')

# Open connection to serial port
uart = serial.Serial(port=SERIAL_PORT, baudrate=SERIAL_BAUD)

# Frobulate the DTR pin to reset the target
uart.setDTR(level=True)
time.sleep(1)
uart.setDTR(level=False)

# Open output file and dump data to it line at a time
with open(OUTPUT_FILE, 'w+') as outf:
    for line in uart:
        outf.write(line)
        outf.flush()

sys.exit(0)
