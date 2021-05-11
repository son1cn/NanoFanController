
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define SERIAL_BAUD 115200 //make sure this is the same in arduino.ino

ESP8266WebServer server(80);

String serialData = "";

//this is our HTML website that we send to the user, much easier to use SPIFFS in future.
const String html_page = "<!DOCTYPE html>"
												 "<html>"
												 "  <head>"
												 "    <meta name='viewport' content='width=device-width, initial-scale=1.0' />"
												 "  </head>"
												 "  <body>"
												 "    <a href='/ledon'>turn LED on</a>"
												 "    <a href='/ledoff'>turn LED off</a>"
												 "    <h1>Latest data from arduino:</h1>"
												 "    <pre id='reading'></pre>"
												 "    <script>"
												 "      (function() {"
												 "        /* get new data every second*/"
												 "        setInterval(function() {"
												 "          fetch('/reading')"
												 "					.then(response => { return response.text();})"
												 "					.then(text => {"
												 "            document.getElementById('reading').textContent = text;"
												 "          });"
												 "        }, 100);"
												 "      })();"
												 "    </script>"
												 "  </body>"
												 "</html>";

const IPAddress serverIPAddress(10, 0, 0, 7);

void setup(void)
{

	Serial.begin(SERIAL_BAUD);

	//here we set up as a hot spot, called XC4411 dual board
	WiFi.softAPConfig(serverIPAddress, serverIPAddress, IPAddress(255, 255, 255, 0));
	WiFi.softAP("XC4411 Dual Board example code");

	//here we set server paramters, the main page is the html_page from above
	server.on("/", []() { //send html code, from above
		server.send(200, "text/html", html_page);
	});

	//reading is just a copy of the serial Data
	server.on("/reading", []() { //send raw serial data
		Serial.println();
		Serial.println(serialData);
		server.send(200, "text/plain", serialData);
	});

	server.on("/ledon", []() {
		Serial.println("[LEDON]"); //send serial data to arduino
		server.send(200, "text/plain", "ok");
	});
	server.on("/ledoff", []() {
		Serial.println("[LEDOFF]"); //send serial data to arduino
		server.send(200, "text/plain", "ok");
	});
	server.on("/ledtoggle", []() {
		Serial.println("[LEDTOGGLE]"); //send serial data to arduino
		server.send(200, "text/plain", "ok");
	});

	server.onNotFound(handleNotFound);
	server.begin();
}

void loop(void)
{
	while (Serial.available())
	{
		char x = Serial.read();
		if (x == '\r')
			continue;
		serialData += x;
	}

	server.handleClient();
}

void handleNotFound()
{
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++)
	{
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
	}
	server.send(404, "text/plain", message);
}
