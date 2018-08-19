#include <SD.h>
#include "RTClib.h"
#include <Wire.h>

RTC_PCF8523 rtc;

const int ledPin=13;
const int buttonPin = 11;
int buttonState = 0;
int stage = 0;
int timeInMin=0;
int flightSched[] = {10,11,12,13,1000};//{45, 195, 555, 675, 10000}; // 45 150 360 (valve 1+2 open) 120 (valve 1+3 open)
String stageNames[5]={"On the ground, captain", "Beam Me Up, Scotty", "She's giving all she got", "Let's replace the thrusters", "Calm of the Wind"};

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
  pinMode(ledPin, OUTPUT);
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    // ADD THREE VALVE CLICK TEST
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

  Serial.print("Stage ");
  Serial.print(stage);
  Serial.print(': ');
  Serial.print(stageNames[stage]);
  Serial.println();
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

 delay(4000); 


//REMOVE LED CODE BEFORE LAUNCH
 for(int j=0;j<stage;j++){
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(500);
 }
}
