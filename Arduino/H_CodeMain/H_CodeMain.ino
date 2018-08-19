#include <SD.h>
#include "RTClib.h"
#include <Wire.h>

RTC_PCF8523 rtc;

const int buttonPin = 11;
int buttonState = 0;
int stage = 0;
int timeInMin=0;
int flightSched[] = {45, 195, 555, 675}; // 45 150 360 (valve 1+2 open) 120 (valve 1+3 open)

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

  //Initialize Button
  pinMode(buttonPin, INPUT);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    //reset RTC
    rtc.adjust(DateTime(2018, 1, 1, 0, 0, 0));
  }

  DateTime now = rtc.now();

  timeInMin = now.hour()*60 + now.minute();

  for(int i=0;i<(sizeof(flightSched)/sizeof(int));i++)
  {
    if (timeInMin < flightSched[i])
    {
      stage = i;
      break;
    }
  }

  Serial.print("stage: ");
  Serial.print(stage);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

 delay(10000); 
}
