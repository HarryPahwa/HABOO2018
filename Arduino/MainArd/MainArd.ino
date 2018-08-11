#include <Adafruit_Sensor.h>
#include "RTClib.h"
#include <Wire.h>
#include <EEPROM.h>
RTC_PCF8523 rtc;
int EEPROMReset = 11;
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
void EEPROMWritelong(int address, long value) // Function for writing the reference time into the EEPROM
      {
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);
      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address, four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      }
long EEPROMReadlong(long address) // Function to read the values stored at a specific address in the EEPROM
      {
      //Read the 4 bytes from the eeprom memory.
      long four = EEPROM.read(address);
      long three = EEPROM.read(address + 1);
      long two = EEPROM.read(address + 2);
      long one = EEPROM.read(address + 3);
      //Return the recomposed long by using bitshift.
      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
      }
      
// The following function will wait until the desired altitude is reached and then set the EEPROM address10 = 0
 void GroundTimer()
 {
       DateTime now = rtc.now();
  
       if (now.unixtime() - GroundRefTime == GroundWaitTime) // Waits until the Balloon lifts off, writes reference time to EEPROM, sets address10 = 0
       {
        EEPROMWritelong(address,now.unixtime()); // Writes time based on the real time clock 
        ReferenceTime = EEPROMReadlong(address);
        EEPROM.write(10,0);
       }
 }
void setup()
{
Serial.begin(57600);
pinMode(relayPin1, OUTPUT); // Setting Relay pin 1 to output
pinMode(relayPin2, OUTPUT); // Setting Relay pin 2 to output
pinMode(relayPin3, OUTPUT); // Setting Relay pin 3 to output
pinMode(EEPROMReset, INPUT); // Setting the EEPROM reset button pin to input
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
 if(digitalRead(EEPROMReset) == HIGH) //Pressing button will initialize the EEPROM for flight
{ 
 EEPROM.write(10,1);
 GroundRefTime = now.unixtime();
 EEPROMWritelong(20,GroundRefTime);
}
  GroundTimer(); // Runs until the desired balloon lifts off and then writes the reference time to the EEPROM
  
  //DateTime now = rtc.now(); 
  
  if(EEPROM.read(10) == 0);
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
