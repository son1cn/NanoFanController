Starting with Arduino IDE (1.8.1 via snap on Ubuntu 24.04)

IDE might fight you to move things to their own folders, feel free.

# ESP Flashing
Add ESP8266 boards to Arduino IDE (File -> Preferences) Additional Boards Manager URLS: http://arduino.esp8266.com/stable/package_esp8266com_index.json

Tools -> Boards -> Board Manager
ESP8266
Need V3.0.2 due to python 3.7 issues within snap 
[See this issue for details on V3.0.2](https://github.com/snapcrafters/arduino/issues/33)

Select Board -> ESP8266 -> Generic ESP8266 Module
Click Verify to that the ESP code builds (i.e. libraries where they should be)

Follow the /Uno+ESP8266/Docs/XC4411-manualMain.pdf guide for DIP switches necessary for flashing (hint: 5,6,7), then put back to 1,2 for normal operation

Upload ESP code to board, reset DIP switches after.