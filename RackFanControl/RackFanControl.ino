#include "DHT.h"

#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)

//int maxHum = 60;
int maxTemp = 40;

DHT dht(DHTPIN, DHTTYPE);
//#include <avr/wdt.h>
#include <EEPROM.h>

//#define SENSOR A7 //Change this to A0 if using an Arduino Uno

const char MAGIC[]="FANC3";

float SENSOR_CALIBRATION=7.55f,
      TEMP_MIN_1=25.0f, TEMP_MAX_1=50.0f, CURVE_1=0.7f;
bool debugging=false, manual=false;
float temp=0,f1=0,f2=0,f3;
uint16_t raw=0;
unsigned long ts=0;


void setup() {
  //Serial.begin(9600); 
  dht.begin();
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  //wdt_enable(WDTO_2S);
  if(debugging) Serial.println(F("FAN CONTROLLER"));
  pinMode(9,OUTPUT);
  loadSettings();
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
  // Wait a few seconds between measurements.
  //Serial.println(F("entering loop"));
  //wdt_reset();
  delay(20000);
  //Serial.println(F("post delay"));
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  //float h = dht.readHumidity();

  //Serial.println(F("post humidity"));
  // Read temperature as Celsius
  temp = dht.readTemperature();
  //Serial.println(F("post temp"));

  //printConfig();
  
  /* Check if any reads failed and exit early (to try again).
  if (isnan(temp)){//isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }*/
  
  //Serial.print("Humidity: "); 
  //Serial.print(h);
  //Serial.print(" %\t");
  //Serial.print("Temperature: "); 
  //Serial.print(temp);
  //Serial.println("*C ");
  if(ts==0||millis()-ts>=1000){
    //raw=analogRead(SENSOR);
    //if(!manual) temp=(float)raw/SENSOR_CALIBRATION;
    f1=(float)(temp-TEMP_MIN_1)/(TEMP_MAX_1-TEMP_MIN_1);
    f1=0;
    //f2=(float)(temp-TEMP_MIN_2)/(TEMP_MAX_2-TEMP_MIN_2);
    //f3=(float)(temp-TEMP_MIN_3)/(TEMP_MAX_3-TEMP_MIN_3);
    f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
    //Serial.print("f1:");
    //Serial.println(f1);
    //f2=pow(f2<0?0:f2>1?1:f2,CURVE_2);
    //f3=pow(f3<0?0:f3>1?1:f3,CURVE_3);
    OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
    //OCR1B = (uint16_t)(320*f2); //set PWM width on pin 10
    //OCR2B = (uint8_t)(79*f3); //set PWM width on pin 3
    if(debugging) printStatus();
    ts=millis();
  }else delay(100);
  /*f1=0.20f;
  f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
  //Serial.print("f1:");
  //Serial.println(f1);
  OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
  delay(10000);
  f1=0.30f;
  f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
  //Serial.print("f1:");
  //Serial.println(f1);
  OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
  delay(10000);
  f1=0.40f;
  f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
  //Serial.print("f1:");
  //Serial.println(f1);
  OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
  delay(10000);
  f1=0.50f;
  f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
  //Serial.print("f1:");
  //Serial.println(f1);
  OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
  delay(10000);
  f1=0.60f;
  f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
  //Serial.print("f1:");
  //Serial.println(f1);
  OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
  delay(10000);
  f1=0.70f;
  f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
  //Serial.print("f1:");
  //Serial.println(f1);
  OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
  delay(10000);
  f1=0.80f;
  f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
  //Serial.print("f1:");
  //Serial.println(f1);
  OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
  delay(10000);
  f1=0.90f;
  f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
  //Serial.print("f1:");
  //Serial.println(f1);
  OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
  delay(10000);
  f1=1.0f;
  f1=pow(f1<0?0:f1>1?1:f1,CURVE_1);
  //Serial.print("f1:");
  //Serial.println(f1);
  OCR1A = (uint16_t)(320*f1); //set PWM width on pin 9
  delay(20000);*/
}

bool loadSettings(){
  if(debugging) Serial.print(F("Loading settings... "));
  uint16_t eadr=0;
  auto eepromReadFloat=[&](){
      float f;
      EEPROM.get(eadr,f);
      eadr+=sizeof(f);
      return f;
  };
  bool magicOk=true;
  char m[sizeof(MAGIC)];
  EEPROM.get(eadr,m);
  if(strcmp(m,MAGIC)!=0){
    if(debugging) Serial.println(F("default"));
    return false;
  }
  eadr+=sizeof(MAGIC);
  SENSOR_CALIBRATION=eepromReadFloat();
  TEMP_MIN_1=eepromReadFloat();
  TEMP_MAX_1=eepromReadFloat();
  CURVE_1=eepromReadFloat();
  EEPROM.get(eadr,debugging);
  eadr+=sizeof(debugging);
  if(debugging){
    Serial.println(F("loaded"));
    printConfig();
  }
  return true;
}

void printConfig(){
  Serial.print(SENSOR_CALIBRATION);
  Serial.print(F(" "));
  Serial.print(TEMP_MIN_1);
  Serial.print(F(" "));
  Serial.print(TEMP_MAX_1);
  Serial.print(F(" "));
  Serial.print(CURVE_1);
  Serial.print(F(" "));
}

void printStatus(){
  Serial.print(F("Fan1:"));
  Serial.println(f1*100.0f);
}
