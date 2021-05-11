Project to use Arduino and 12V PWM fans to control temperature for network rack

Information gathered from:

	DHT input
		Adafruit library
	Fan PWM output using Arduino
		https://github.com/adolfintel/NanoFanController
	

Originally built with:
<pre>
DHT temp -> Arduino Nano -> 12V PWM fan
	          | Serial over USB
	       Rasp Pi -> Prometheus Endpoint
				|
		    Network Prometheus Control -> Grafana
</pre>
![arduinoNano cropped](https://user-images.githubusercontent.com/31121758/117897030-067b1180-b301-11eb-951e-97c28011d19e.jpg)

After breaking my Arduino Nano (likely shorted out on something inside rack, I did not protect all the Arduino pins)
I bought an Arduino Uno that included an ESP8266 Wifi module. The setup now looks like:
<pre>

DHT temp -> Arduino Uno-> 12V PWM fan
	         | on-board serial
	      ESP8266 -> Prometheus Endpoint
				|
		    Network Prometheus Control -> Grafana
</pre>
Here is a photo before making the header breakout board more permanent
![ArduinoUno controller](https://user-images.githubusercontent.com/31121758/117897397-e009a600-b301-11eb-803a-f176564919f2.jpg)

The new setup lets the fan control arduino only need power and is isolated from the Raspberry Pis within the network rack
