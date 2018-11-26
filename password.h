#ifndef password1234
#define password1234

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <EEPROM.h>

class password{
  String pass1;
  byte EEPROM_ADDRESS;

  public:
  password(byte);
  void initP();
  bool checkPass(String);

//  void PP();
};


#endif
