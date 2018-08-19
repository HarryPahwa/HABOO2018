#include <SD.h>
#include "RTClib.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <TimeLib.h>
#include <EEPROM.h>

//#define BME_SCK 13
//#define BME_MISO 12
//#define BME_MOSI 11
//#define BME_CS 10

#define SEALEVELPRESSURE_HPA (1013.25)

//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI
Adafruit_BME280 bme;

RTC_PCF8523 rtc;

const int ledPin=13; //REMOVE FOR MAIN FLIGHT
const int buttonPin = 4; //RESET BUTTON

int buttonState = 0;
int stage = 0;
int timeInMin=0;
int flightSched[5] = {1,2,3,4,10000};//{45, 195, 555, 675, 10000}; // 45 150 360 (valve 1+2 open) 120 (valve 1+3 open)
String stageNames[5]={"On the ground, captain", "Beam Me Up, Scotty", "She's giving all she got", "Let's replace the thrusters", "Calm of the Wind"};

byte relayPin[4] = {2,7,8,10};

 //D2 -> RELAY1 [Valve1]
 //D7 -> RELAY2 [Valve2]
 //D8 -> RELAY3 [Valve3]
 //D10 -> RELAY4 [NoValve]

//ADD SD CARD STUFF HERE

void setup() {
  // put your setup code here, to run once:
  while (!Serial) {
    delay(1);  // for Leonardo/Micro/Zero
  }
  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(2018, 1, 1, 0, 0, 0));
  }

  //Initialize Button/LED
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT); //REMOVE BEFORE ACTUAL FLIGHT
  
  for(int i = 0; i < 4; i++)  pinMode(relayPin[i],OUTPUT);

  bool status;
  status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        
    }

  loadCurrentTime();
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    // ADD THREE VALVE CLICK TEST
    Serial.println("RESET RESET RESET");
    delay(2000);
    for(int i = 0; i < 4; i++)  {
     digitalWrite(relayPin[i],HIGH);
     delay(500);
     digitalWrite(relayPin[i],LOW);
     delay(500);
   }
    
    //reset RTC
    //rtc.adjust(DateTime(2018, 1, 1, 0, 0, 0));
    setTime(0, 0, 0, 1, 1, 2018);
  }

  //DateTime now = rtc.now();

  timeInMin = hour()*60 + minute();

  for(int i=0;i<(sizeof(flightSched)/sizeof(int));i++)
  {
    if (timeInMin < flightSched[i])
    {
      stage = i;
      break;
    }
  }


  switch (stage) {
    case 0:
    case 1:
      for(int i=0;i<4;i++) digitalWrite(relayPin[i],LOW);
      break;
    case 2:
      digitalWrite(relayPin[0],HIGH);
      digitalWrite(relayPin[1],HIGH);
      digitalWrite(relayPin[2],LOW);
      digitalWrite(relayPin[3],LOW);
      break;
    case 3:
      digitalWrite(relayPin[0],HIGH);
      digitalWrite(relayPin[1],LOW);
      digitalWrite(relayPin[2],HIGH);
      digitalWrite(relayPin[3],LOW);
      break;
    case 4:
      for(int i=0;i<4;i++) digitalWrite(relayPin[i],LOW);
      break;
  }
  




//PRINTING JARGON -> USELESS STUFF

  Serial.print("Stage ");
  Serial.print(stage);
  Serial.print(": ");
  Serial.print(stageNames[stage]);
  Serial.println();
  Serial.print(year(), DEC);
  Serial.print('/');
  Serial.print(month(), DEC);
  Serial.print('/');
  Serial.print(day(), DEC);
  Serial.print(" (");
  
  Serial.print(") ");
  Serial.print(hour(), DEC);
  Serial.print(':');
  Serial.print(minute(), DEC);
  Serial.print(':');
  Serial.print(second(), DEC);
  Serial.println();
  printValues();
  delay(5000); 
  saveCurrentTime();

//REMOVE LED CODE BEFORE LAUNCH
 for(int j=0;j<stage;j++){
  digitalWrite(ledPin, HIGH);
  delay(100);
  
  digitalWrite(ledPin, LOW);
  delay(100);
 }
}

void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}

void saveCurrentTime() {
  EEPROM_writeInt(9, hour()); //Writes the hour in the position 9 of the EEPROM 
  EEPROM_writeInt(11, minute()); //Writes the minutes in the position 11 of the EEPROM 
  EEPROM_writeInt(13, second()); //Writes the seconds in the position 13 of the EEPROM 
}

void EEPROM_writeInt(int ee, int value)
{
    byte* p = (byte*)(void*)&value;
    for (int i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);
}

void loadCurrentTime() {  
    setTime(EEPROM_readInt(9,2),EEPROM_readInt(11,2),EEPROM_readInt(13,2),1,1,2018);
}

int EEPROM_readInt(int ee, int size)
{
    int value = 0.0;
    byte* p = (byte*)(void*)&value;
    for (int i = 0; i < size; i++)
        *p++ = EEPROM.read(ee++);
    return value;
}

