#include "DHT.h"
#include <stdio.h>

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

int maxTemp = 40;

DHT dht(DHTPIN, DHTTYPE);

float TEMP_MIN_1=25.0f, TEMP_MAX_1=50.0f, CURVE_1=0.7f;
bool debugging=false;
float temp=0,f1=0;
unsigned long ts=0;
char buf[14], tempf[5], fanf[5];

void setup() {
  Serial.begin(9600); 
  dht.begin();
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  if(debugging) Serial.println(F("FAN CONTROLLER"));
  pinMode(9,OUTPUT);
  //Set PWM frequency to about 25khz on pins 9,10 (timer 1 mode 10, no prescale, count to 320)
  TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
  TCCR1B = (1 << CS10) | (1 << WGM13);
  ICR1 = 320;
  //startup pulse
  if(debugging) Serial.print(F("Startup pulse... "));
  OCR1A = 320;
  delay(1000);
  OCR1A = 0;
  if(debugging) Serial.println(F("done"));
  digitalWrite(LED_BUILTIN,LOW);
}

void loop() {
  // Read temperature as Celsius
  temp = dht.readTemperature();
  //force temp to be string with 2 decimals
  dtostrf(temp,4,2,tempf);
  
  if(ts==0||millis()-ts>=1000){
    f1=(float)(temp-TEMP_MIN_1)/(TEMP_MAX_1-TEMP_MIN_1);
    f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
	//force fan speed to be string with 2 decimals
    dtostrf(f1*100.0f,4,2,fanf);
	//format serial output to "T:xx.xxF:yy.yy"
    sprintf(buf,"T:%sF:%s",tempf,fanf);
	//send serial output
    Serial.println(buf);
    OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
    ts=millis();
  }else delay(100);

  //wait 5 sec before starting loop again
  delay(5000);
}
