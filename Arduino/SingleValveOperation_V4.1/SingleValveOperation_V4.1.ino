#include <SD.h>
#include "RTClib.h"
#include <Wire.h>

RTC_PCF8523 rtc;
int relayPin1 = 2;
int relayPin2 = 7;
int relayPin3 = 8;
unsigned long GroundWaitTime = 5; //Time spent on the ground before liftoff (in seconds)
unsigned long ClimbTime = 5; // Set this to the desired climb time (in seconds) 
unsigned long ValveOpenTime1 = 10; // Set this to the desired opening time for valve 1 (in seconds) 
unsigned long ValveOpenTime2 = 20; // Set this to the desired opening time for valve 2 (in seconds)
unsigned long ValveOpenTime3 = 30; // Set this to the desired opening time for valve 3 (in seconds)
unsigned long ElapsedTime; // This is a variable that will be calculated after the reference time is stored in the EEPROM address0
unsigned long ReferenceTime; // This is the reference time. It will be stored in the EEPROM address0 once the desired altitude is reached
unsigned long GroundRefTime; // Reference time to calculate time passed on the ground before lift off
int address = 0; // This integer will be called in the EEPROM writing functions down below


// The following function will wait until the desired altitude is reached and then set the EEPROM address10 = 0
 void GroundTimer()
 {
       DateTime now = rtc.now();
  
 }
 
void setup()
{
  Serial.begin(57600);
  pinMode(relayPin1, OUTPUT); // Setting Relay pin 1 to output
  pinMode(relayPin2, OUTPUT); // Setting Relay pin 2 to output
  pinMode(relayPin3, OUTPUT); // Setting Relay pin 3 to output
 if (! rtc.begin()) //Initializing the real time clock on board the SD card shield 
 {
    Serial.println("Couldn't find RTC"); //if the real time clock doesn't function, the arduino will halt
    while (1);
  }
    if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
  } 
}
void loop()
{
  DateTime now = rtc.now();
 

  GroundTimer(); // Runs until the desired balloon lifts off and then writes the reference time to the EEPROM
  
  //DateTime now = rtc.now(); 
  
  

  ElapsedTime = now.unixtime() - ReferenceTime; //This calculates the total time elapsed since the desired altitude was reached 
// This section of the code determines when to open and close valves 
  Serial.println(ElapsedTime);
  if(ElapsedTime > ClimbTime && ElapsedTime <= ValveOpenTime1 )
  {
  digitalWrite(relayPin1,HIGH);
  }
  else if(ElapsedTime > ValveOpenTime1 && ElapsedTime <= ValveOpenTime2)
  {
  digitalWrite(relayPin1,LOW);
  delay(1000);
  digitalWrite(relayPin2,HIGH);
  }
  else if(ElapsedTime > ValveOpenTime2 && ElapsedTime <= ValveOpenTime3)
  {
  digitalWrite(relayPin2,LOW);
  delay(1000);
  digitalWrite(relayPin3,HIGH);
  }
  else if(ElapsedTime > ValveOpenTime3)
  {
  digitalWrite(relayPin3,LOW);
  }
}


