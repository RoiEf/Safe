#include "MyTime.h"

  MyTime::MyTime (int Address){
    eeprom_time_address = Address;
//    eeprom_slope_address += Address;
//    eepromBadAddress += eeprom_slope_address;
//    eepromAttAddress += eepromBadAddress;
    
    read_eeprom_time(Address);
    slope = get_slope();

  }

  MyTime::MTinit(){
    get_rtc_time();
    readBadTrials();
  }
  
  void MyTime::read_eeprom_time (int eeprom_address){
     
    eeprom_time.sec = EEPROM.read(eeprom_address);
    eeprom_time.min = EEPROM.read(++eeprom_address);
    eeprom_time.hour = EEPROM.read(++eeprom_address);
    eeprom_time.mday = EEPROM.read(++eeprom_address);
    eeprom_time.mon = EEPROM.read(++eeprom_address);
    int16_t x = EEPROM.read(++eeprom_address);
    x *= 100;
    eeprom_time.year = EEPROM.read(++eeprom_address);
    eeprom_time.year += x;
  }
  
  void MyTime::write_eeprom_time (int eeprom_address){
     EEPROM.write(eeprom_address,rtc_time.sec);
     EEPROM.write(++eeprom_address,rtc_time.min);
     EEPROM.write(++eeprom_address,rtc_time.hour);
     EEPROM.write(++eeprom_address,rtc_time.mday);
     EEPROM.write(++eeprom_address,rtc_time.mon);
     byte x = rtc_time.year/100;
     EEPROM.write(++eeprom_address,x);
     x = rtc_time.year%100;
     EEPROM.write(++eeprom_address,x);
  }

  void MyTime::get_rtc_time(){
    DS3231_get(&rtc_time);               // Read the time from the RTC module
//    Serial.println("RTC Time got is:");
//    MyTime::printTime(&rtc_time);
  }
/*
  void MyTime::printTime(ts * pt){
    Serial.print("Time: ");
    Serial.print(pt->hour);
    Serial.print(":");
    Serial.print(pt->min);
    Serial.print(":");
    Serial.println(pt->sec);  
    Serial.print("Date: ");
    Serial.print(pt->mday);
    Serial.print("/");
    Serial.print(pt->mon);
    Serial.print("/");
    Serial.println(pt->year);
    }
*/  
  byte MyTime::get_slope(){
//    return int (EEPROM.read(eeprom_slope_address));
     return 2;
  }

  double MyTime::return_delta_time(){
    struct tm * time1;
 //   struct tm * time2;
    double x;
     struct ts rtc_time;                     // used to hold time data from the RTC
     DS3231_get(&rtc_time);               // Read the time from the RTC module
     
    time1->tm_sec = eeprom_time.sec;
//    Serial.print("eeprom_time.sec = ");
//    Serial.println(eeprom_time.sec);
    time1->tm_min = eeprom_time.min;
//    Serial.print("eeprom_time.min = ");
//    Serial.println(eeprom_time.min);
    time1->tm_hour = eeprom_time.hour;
//    Serial.print("eeprom_time.hour = ");
//    Serial.println(eeprom_time.hour);
    time1->tm_mday = eeprom_time.mday;
//    Serial.print("eeprom_time.mday = ");
//    Serial.println(eeprom_time.mday);
    time1->tm_mon = eeprom_time.mon;
//    Serial.print("eeprom_time.mon = ");
//    Serial.println(eeprom_time.mon);
    time1->tm_year = eeprom_time.year;
//    Serial.print("eeprom_time.year = ");
//    Serial.println(eeprom_time.year);
    time_t EEpromTime = mktime(time1);
    Serial.print("time_t EEpromTime = ");
    Serial.println(EEpromTime);

    time1->tm_sec = rtc_time.sec;
    Serial.print("rtc_time.sec = ");
    Serial.println(rtc_time.sec);

    time1->tm_min = rtc_time.min;
    Serial.print("rtc_time.min = ");
    Serial.println(rtc_time.min);
    Serial.print("time1->tm_min = ");
    Serial.println(time1->tm_min);
        
    time1->tm_hour = rtc_time.hour;
    Serial.print("rtc_time.hour = ");
    Serial.println(rtc_time.hour);
    time1->tm_mday = rtc_time.mday;
    Serial.print("rtc_time.mday = ");
    Serial.println(rtc_time.mday);
    time1->tm_mon = rtc_time.mon;
    Serial.print("rtc_time.mon = ");
    Serial.println(rtc_time.mon);
    time1->tm_year = rtc_time.year;    
    Serial.print("rtc_time.year = ");
    Serial.println(rtc_time.year);
    time_t RtcTime = mktime(time1);
    Serial.print("time_t RtcTime = ");
    Serial.println(RtcTime);
    
//    x = difftime(RtcTime, EEpromTime);
//    Serial.println("Start return delta time");
//    Serial.print("delta time is: ");
//    Serial.println(x);
    
    return 200.0;
  }

 
  
  bool MyTime::is_ok(){
    double x;
    
    if(badTrials <=5 ){
//      Serial.print("is_ok = OK      bad trials = ");
//      Serial.println(badTrials);
      return 1;
    } else{
//      Serial.print("is_ok = bad     bad trials = ");
//      Serial.println(badTrials);
      
      MyTime::get_rtc_time();   // time when the function was called
      MyTime::read_eeprom_time(eeprom_time_address);
//      MyTime::printTime(&eeprom_time);
//      MyTime::printTime(&rtc_time);
      x = MyTime::return_delta_time();
//      x /= 60;
//Serial.println(x);
//      if (x >= slope) {
        return 1;
 //     } else {
//        return 1;
 //     }

 
    }
  }

  void MyTime::resetBad(){
    badTrials = 0;
    badAttempts = 0;
    EEPROM.write((eeprom_time_address + eeprom_slope_address + eepromBadAddress),0);
    EEPROM.write((eeprom_time_address + eeprom_slope_address + eepromBadAddress + 1),0);
  }

  void MyTime::updateBad(){
    Serial.println("START MyTime::updateBad START");

    badTrials ++;
    EEPROM.write((eeprom_time_address + eeprom_slope_address + eepromBadAddress),badTrials);
    Serial.print("badTrials = ");
    Serial.println(badTrials);
    Serial.println("After updating, allready in the EEProm");

    Serial.println(" ");
    if(badTrials > 5){
    Serial.println("Since bad trials was greater then 5");
    Serial.print("badAttempts = ");
    Serial.println(badAttempts);
      if(badAttempts <= 2){
        badAttempts++;
        EEPROM.write((eeprom_time_address + eeprom_slope_address + eepromBadAddress + 1),badAttempts);
    Serial.println("Since badAttempts was smaller then 2, increase and update");
    Serial.print("updated badAttempts = ");
    Serial.println(badAttempts);
      } else {
    Serial.println("Since badAttempts was bigger then 2 ");
    Serial.println("badAttempts = 0");
    Serial.println("Also updating in the EEProm");
        badAttempts = 0;
        EEPROM.write((eeprom_time_address + eeprom_slope_address + eepromBadAddress + 1),badAttempts);      
    Serial.println("Getting RTC time and updating into EEProm");
    Serial.println("for next round, in case it will get over 5");
        MyTime::get_rtc_time();
        MyTime::write_eeprom_time(eeprom_time_address);
//        MyTime::read_eeprom_time(eeprom_time_address);
//        MyTime::printTime(&eeprom_time);
      }
    }

    Serial.println("END MyTime::updateBad END");
 }

void MyTime::readBadTrials(){
//    Serial.println("START MyTime::readBadTrials");
  badTrials = EEPROM.read((eeprom_time_address + eeprom_slope_address + eepromBadAddress));
//    Serial.print("badTrials");
//     Serial.println(badTrials);
  badAttempts = EEPROM.read((eeprom_time_address + eeprom_slope_address + eepromBadAddress + 1));
//     Serial.print("badAttempts");
//     Serial.println(badAttempts);
//    Serial.println("END MyTime::readBadTrials");
}

