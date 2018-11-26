//#define DEBUGLEVEL 0
//#include "DebugUtils.h"

#include <Servo.h>
Servo myservo;  // create servo object to control a servo

#include <avr/sleep.h>          // library for the low power sleep mode
#define wakePin 2               // interrupt pin to wake mcu up from deep sleep

#include <Keypad.h>             // matrix keypad library

#include <Wire.h>               // main library for the I2C bus
#include "ds3231.h"             // RTC library
#define BUFF_MAX 128            // used for formatting of the RTC string

#include "timer.h"              // inactivity timer object & functions
#include "MyTime.h"
#include "password.h"           // Library for password stuff

#include <EEPROM.h>             // EEProm library, just to be on the safe side
#define MyTimeEEPromStart 1     // start address in EEPROM where data is
#define slopeAddress 7          // offset of the address in eeprom from previos data
#define timesAddress 2          // offset of the address for bad trials and bad attempts
#define passAddress (MyTimeEEPromStart + slopeAddress + timesAddress +1 )

const byte ROWS = 4; //four rows      // keypad settings
const byte COLS = 3; //three columns  // keypad settings
char keys[ROWS][COLS] = {             // keypad settings
  {'1','2','3'},                      // keypad settings
  {'4','5','6'},                      // keypad settings
  {'7','8','9'},                      // keypad settings
  {'*','0','#'}                       // keypad settings
};                                    // keypad settings
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3}; //connect to the column pinouts of the keypad

Keypad customKeypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );   // starting the keypad object

  MyTime mtObj1(MyTimeEEPromStart);         // object for the security timer
  timer timer1(5);                            // object for the inactivity timer set to 2 minuets
  password passObj1(passAddress);             // Object for the password

  String str2;                  // used to hold the inputed password
  int n = 0;                    // remember how many charecters inputed in the string of the password

void setup(){
  Serial.begin(9600);           // starting serial communication
  Wire.begin();                 // starting I2C bus
  DS3231_init(DS3231_INTCN);    // initialising RTC object
  
  mtObj1.MTinit();              // 
  passObj1.initP();             // intialising the password object
    
  pinMode(wakePin, INPUT_PULLUP);         // setting wake up interrupt pin to input.
  myservo.attach(11);  // attaches the servo on pin 11 to the servo object
        
  while(!Serial);               // making sure serial communication work
  Serial.println("Here we go");

//  passObj1.PP();                // print cuttent password to serial for debbuging
  myservo.write(0);             // erset the servo
}


void loop(){
//  Serial.println("Loop start");

    char key = customKeypad.getKey();

    if (key != NO_KEY){
 //   Serial.println(key);
    
  if (mtObj1.is_ok()) {
      str2.concat(key);
    Serial.print("Password string is: ");
    Serial.println(str2);
      n++;
      if (n >= 100) {            // protection for crashing the processor with too long password
        str2 = String("");
        n = 0;
      }

     if (key == '#') {                             // if user entered "#" for finishing pressing in password
        if(passObj1.checkPass(str2)){               // check if password is correct
                                                  // if password was correct
          str2 = String("");
          n = 0;
          mtObj1.resetBad();
          timer1.resetTimer();
        
          Serial.println("Opening Door");
          Serial.print("Inactivity timer reset at: ");
//          justPrintTime();

          myservo.write(90);
          delay(10000);
          myservo.write(0);
          delay(3000);
          Serial.print("Door locked again. going to sleep at: ");
//          justPrintTime();    
          sleepNow();                         // put system to sleep
          
      } else {                                    // if password was not correct
          str2 = String("");      // clear the password input string. waiting for the next input.
          n = 0;                  // zero the counter of how many characters inputed
          mtObj1.updateBad();     // updates the EEProm about the faild access attempt
          Serial.println("Password was incorrect");
          Serial.println("NOT Opening Door");
        }
      }
    } //  end mtObj1.is_ok();
     
    Serial.println(key);
    Serial.print("Inactivity timer reset at: ");
    justPrintTime();
    timer1.resetTimer();
 }

  if (timer1.checkInterval()){       // if inactivity timer run out of time,
    Serial.print("Inactivity runout at: ");
//    justPrintTime();    
    sleepNow();                         // put system to sleep
  }
}

void wakeUpNow()        // here the interrupt is handled after wakeup
{
  // Prevent sleep mode, so we don't enter it again, except deliberately, by code
  sleep_disable();

  // Detach the interrupt that brought us out of sleep
  detachInterrupt(digitalPinToInterrupt(wakePin));
  
  // Resetting inactivity timer
  timer1.resetTimer();
}

void sleepNow()         // here we put the arduino to sleep
{
    // Send a message just to show we are about to sleep
    Serial.println("Good night!");
    Serial.flush();
    
    // Disable the ADC (Analog to digital converter, pins A0 [14] to A5 [19])
    static byte prevADCSRA = ADCSRA;
    ADCSRA = 0;

    /* Set the type of sleep mode we want. Can be one of (in order of power saving):
        SLEEP_MODE_IDLE (Timer 0 will wake up every millisecond to keep millis running)
        SLEEP_MODE_ADC
        SLEEP_MODE_PWR_SAVE (TIMER 2 keeps running)
        SLEEP_MODE_EXT_STANDBY
        SLEEP_MODE_STANDBY (Oscillator keeps running, makes for faster wake-up)
        SLEEP_MODE_PWR_DOWN (Deep sleep)
    */
    set_sleep_mode (SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    // Turn of Brown Out Detection (low voltage)
    // Thanks to Nick Gammon for how to do this (temporarily) in software rather than
    // permanently using an avrdude command line.
    //
    // Note: Microchip state: BODS and BODSE only available for picoPower devices ATmega48PA/88PA/168PA/328P
    //
    // BODS must be set to one and BODSE must be set to zero within four clock cycles. This sets
    // the MCU Control Register (MCUCR)
    MCUCR = bit (BODS) | bit (BODSE);

    // The BODS bit is automatically cleared after three clock cycles so we better get on with it
    MCUCR = bit (BODS);

    // Ensure we can wake up again by first disabling interupts (temporarily) so
    // the wakeISR does not run before we are asleep and then prevent interrupts,
    // and then defining the ISR (Interrupt Service Routine) to run when poked awake
    noInterrupts();
    attachInterrupt(digitalPinToInterrupt(wakePin), wakeUpNow, LOW);

    // Allow interrupts now
    interrupts();

    // And enter sleep mode as set above
    sleep_cpu();

    // --------------------------------------------------------
    // µController is now asleep until woken up by an interrupt
    // --------------------------------------------------------

    // Wakes up at this point when wakePin is brought LOW - interrupt routine is run first
    Serial.println("I'm awake!");
    Serial.println("PB press interrupt wakeup.");
    
    Serial.print("Inactivity timer was reset at: ");
//    justPrintTime();

    // Re-enable ADC if it was previously running
    ADCSRA = prevADCSRA;
}

void justPrintTime(){

  char buff[BUFF_MAX];
  struct ts t;                    // used to read from the RTC    
  DS3231_get(&t);               // Read the time from the RTC module

        // there is a compile time option in the library to include unixtime support
#ifdef CONFIG_UNIXTIME
        snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d %ld", t.year,
             t.mon, t.mday, t.hour, t.min, t.sec, t.unixtime);
#else
        snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,
             t.mon, t.mday, t.hour, t.min, t.sec);
#endif

    Serial.println(buff); 

}

