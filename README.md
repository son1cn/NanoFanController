Items to do:

	DHT input
		Adafruit library
	Fan PWM output
		https://github.com/adolfintel/NanoFanController
	Control Loop
		https://fdossena.com/?p=ArduinoFanControl/nano.md
	
Originally built with 
DHT temp -> Arduino Nano -> 12V fan
			   |
	  Rasp Pi (via USB serial) -> Prometheus Endpoint
										|
								Network Prometheus Control -> Grafana

After breaking my Arduino Nano, I bought an Arduino Uno that included an ESP8266 Wifi module. The setup now looks like:
DHT temp -> Arduino Nano -> 12V fan
			   |
	        ESP8266 -> Prometheus Endpoint
							|
				Network Prometheus Control -> Grafana