#ifndef mytime
#define mytime

#if (ARDUINO >=100)
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "ds3231.h"             // RTC library
#include <EEPROM.h>

#include <time.h>
#define eeprom_slope_address 7           // address for the slope modefire of the time delta calculation
#define eepromBadAddress 1
#define eepromAttAddress 1
  
class MyTime {
  struct ts eeprom_time;                  // used to hold time data from the eeprom
  struct ts rtc_time;                     // used to hold time data from the RTC
  int eeprom_time_address;                // start address of the time data in the EEPROM
  
  byte slope = 0;

  int badTrials = 0;                    // remembering the number of times the wrong password was inputed.
  byte badAttempts = 0;                  // remembering up to 3 attempts before relock.

  void read_eeprom_time(int);                
  void write_eeprom_time (int);
  void printTime(ts *);
  void get_rtc_time();
  byte get_slope();
  double return_delta_time();
  void readBadTrials();

public:

  MyTime(int);
  MTinit();
  void resetBad();
  bool is_ok();
  void updateBad();
};

#endif
