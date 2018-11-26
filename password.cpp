#include "password.h"

password::password(byte Address){
  EEPROM_ADDRESS = Address;
  pass1 = String("");
}

void password::initP(){
  byte n = EEPROM_ADDRESS;
  char ch = EEPROM.read(n);
  pass1 = String(ch);
  n++;
  
  while(ch != '#'){
    ch = EEPROM.read(n);
    pass1.concat(ch);
    n++;
  }
}

bool password::checkPass(String pass2) {
  return pass1.equals(pass2);
  
}
/*
void password::PP(){                // this is only for debugging
  Serial.print("Password is: ");
  Serial.println(pass1);
}
*/
