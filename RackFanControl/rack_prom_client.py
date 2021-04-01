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
# Create a metric to track time spent and requests made.
REQUEST_TIME = Summary('request_processing_seconds', 'Time spent processing request')
temp = Gauge('rack_temp', 'Rack Temperature')
fan = Gauge('fan_speed', 'Rack Fan %')

# Decorate function with metric.
@REQUEST_TIME.time()
def process_request(t):
    """A dummy function that takes some time."""
    time.sleep(t)

if __name__ == '__main__':
    # Start up the server to expose the metrics.
    start_http_server(8000)
    # Generate some requests.
    while True:
        #process_request(random.random())
        #x=ser.read(ser.in_waiting)
        x = ser.readline().strip()
        #x = x.decode(encoding='UTF-8')
        if x == b'':
            time.sleep(1)
            continue
        else:
            #print ("T: " + x[x.find("T:")+2:x.find("T:")+7] + " F: " + x[x.find("F:")+2:x.find("F:")+7])
            x = x.decode(encoding='UTF-8')
            #print(x)
            #print(Decimal(x[x.find("F:")+2:x.find("F:")+7]))
            temp.set(Decimal(x[x.find("T:")+2:x.find("T:")+7]))
            fan.set(Decimal(x[x.find("F:")+2:x.find("F:")+7]))
        #print x
