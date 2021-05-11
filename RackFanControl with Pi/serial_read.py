#!/usr/bin/env python
import time
import serial

#on rasp-pi USB serial device can be found using dmesg | grep tty

ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)

while 1:
        x=ser.read(ser.in_waiting)
	if x == b'':
		time.sleep(1)
		continue
	else:
		print "T: " + x[x.find("T:")+2:x.find("T:")+7] + " F: " + x[x.find("F:")+2:x.find("F:")+7]
        #print x
