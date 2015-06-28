#!/usr/bin/env python

from datetime import datetime
import serial
import sys
import time

SERIAL_BAUD = 9600
SERIAL_PORT = '/dev/ttyUSB0'
OUTPUT_FILE = datetime.now().strftime('data/%Y%m%d_%H%M%s_data.csv')

# Check if user wants to reset the port before opening
if (len(sys.argv) > 1 and sys.argv[1] in ('--reset', '-r')):
    DO_RST = True
else:
    DO_RST = False

# Open serial dong
print 'opening serial port %s...' % SERIAL_PORT
uart = serial.Serial(
    port=SERIAL_PORT,
    baudrate=SERIAL_BAUD,
    #timeout=5,
    dsrdtr=DO_RST,
)

# Frobulate the DTR pin to reset the target
if DO_RST:
    print 'twiddling DTR to reset'
    uart.setRTS(False)
    uart.setDTR(False)
    uart.flush()
    time.sleep(0.2)
    uart.flushInput()
    uart.setRTS(True)
    uart.setDTR(True)
    time.sleep(1)
    print 'reset done'

# Send start command to begin cycle
time.sleep(2)
uart.write("START\r\n")
uart.flush()
time.sleep(1)

#print 'discarding crap'
#uart.timeout = 0
#while uart.read() in ('\0', '\n', '\r'):
#    time.sleep(0.1)
#uart.timeout = None

# Open output file and dump data to it line at a time
with open(OUTPUT_FILE, 'w+') as outf:

    print 'actually writing data now'
    try:
        for line in uart:
            outf.write(line)
            outf.flush()
    except KeyboardInterrupt, ke:
        print 'Interrupted!'

print 'done!'
sys.exit(0)
