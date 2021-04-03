#!/usr/bin/env python3
from prometheus_client import start_http_server, Summary, Gauge
from decimal import Decimal
import random
import time
import serial


ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate = 9600,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        bytesize=serial.EIGHTBITS,
        timeout=1
)
# Create the metrics we want to see in Prometheus
temp = Gauge('rack_temp', 'Rack Temperature')
fan = Gauge('fan_speed', 'Rack Fan %')

if __name__ == '__main__':
    # Start up the server to expose the metrics.
    start_http_server(8000)
    # Generate some requests.
    while True:
        #read from serial and strip the end of line chars
        x = ser.readline().strip()
        #wait if nothing has through on serial
        if x == b'':
            time.sleep(1)
            continue
        else:
            #test code to see if find logic is working
            #print ("T: " + x[x.find("T:")+2:x.find("T:")+7] + " F: " + x[x.find("F:")+2:x.find("F:")+7])
            
            #take char array and decode to string
            x = x.decode(encoding='UTF-8')
            #set the metrics
            temp.set(Decimal(x[x.find("T:")+2:x.find("T:")+7]))
            fan.set(Decimal(x[x.find("F:")+2:x.find("F:")+7]))
