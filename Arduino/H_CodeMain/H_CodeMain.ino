#include <SD.h>
#include "RTClib.h"
#include <Wire.h>

RTC_PCF8523 rtc;

const int ledPin=13; //REMOVE FOR MAIN FLIGHT
const int buttonPin = 11; //RESET BUTTON

int buttonState = 0;
int stage = 0;
int timeInMin=0;
int flightSched[5] = {10,11,12,13,1000};//{45, 195, 555, 675, 10000}; // 45 150 360 (valve 1+2 open) 120 (valve 1+3 open)
String stageNames[5]={"On the ground, captain", "Beam Me Up, Scotty", "She's giving all she got", "Let's replace the thrusters", "Calm of the Wind"};

byte relayPin[4] = {2,7,8,10};

 //D2 -> RELAY1 [Valve1]
 //D7 -> RELAY2 [Valve2]
 //D8 -> RELAY3 [Valve3]
 //D10 -> RELAY4 [NoValve]


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

}

void loop() {
  // put your main code here, to run repeatedly:
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    // ADD THREE VALVE CLICK TEST
    Serial.println("RESET RESET RESET");
    delay(2000);
    for(i = 0; i < 4; i++)  {
     digitalWrite(relayPin[i],HIGH);
     delay(500);
     digitalWrite(relayPin[i],LOW);
     delay(500);
   }
    
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
