#include <EEPROM.h>             // EEProm library, just to be on the safe side
#define MyTimeEEPromStart 1      // start address in EEPROM where data is
int slopeAddress = MyTimeEEPromStart + 7;
int timesAddress = slopeAddress + 1;
int passAddress = timesAddress +2;
String pass1;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Here we go");

  EEPROM.write((timesAddress+1),'0'); // set attempts to '0'
  EEPROM.write((timesAddress),'0'); // set attempts to '0'
  EEPROM.write(passAddress,'1');
  EEPROM.write((passAddress+1),'2');
  EEPROM.write((passAddress+2),'3');
 EEPROM.write((passAddress+3),'4');
  EEPROM.write((passAddress+4),'5');
  EEPROM.write((passAddress+5),'#');

Serial.println("EEProm was updated");
  
}

void loop() {

  Serial.println("Loop start");
  int n = passAddress;
  Serial.print("current address: ");
  Serial.println(n);
  char ch = char(EEPROM.read(n));
  Serial.print("first Char read: ");
  Serial.println(ch);

  pass1 = String(ch);
  Serial.print("first pass string: ");
  Serial.println(pass1);
  
  n++;
  Serial.print("current address: ");
  Serial.println(n);

    
  while(ch != '#'){
  ch = EEPROM.read(n);
  Serial.print("Char read: ");
  Serial.println(ch);
  
  pass1.concat(ch);
  Serial.print("current pass string: ");
  Serial.println(pass1);
  
  n++;
  Serial.print("current address: ");
  Serial.println(n);

  }

  delay(1000);
}
