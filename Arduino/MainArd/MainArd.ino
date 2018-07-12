byte relayPin[4] = {2,7,8,10};
 
//D2 -> RELAY1 [Valve1]
//D7 -> RELAY2 [Valve2]
//D8 -> RELAY3 [Valve3]
//D10 -> RELAY4 [NoValve]
 
void setup(){
  Serial.begin(9600);
  for(int i = 0; i < 4; i++)  pinMode(relayPin[i],OUTPUT);
}
 
// an sample to switch the 4 relays
 
void loop(){
   
  int i;
  for(i = 0; i < 4; i++)  {
    digitalWrite(relayPin[i],HIGH);
    delay(500);
    digitalWrite(relayPin[i],LOW);
    delay(500);
  }
  
  Serial.print(digitalRead(relayPin[3]));
  delay(1000);
  for(i = 0; i < 4; i++)  digitalWrite(relayPin[i],LOW);
  Serial.print(digitalRead(relayPin[3]));
  delay(1000);
}
