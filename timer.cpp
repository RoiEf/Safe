#include "timer.h"

timer::timer(){
  delta = 60000*20;   // 20 min delta
  startMillis = millis();       //initial start time for DwD
}
timer::timer(float t){
  delta = long(60000*t);   // ?min delta
  startMillis = millis();       //initial start time for DwD
}

void timer::setDelta(float t){
  delta = long(60000*t);   // ?min delta
}

void timer::resetTimer(){
  startMillis = millis();
}

bool timer::checkInterval(){
  currentMillis = millis();
  if (currentMillis - startMillis >= delta)  //test whether the period has elapsed
  {
    return 1;
  } else {
    return 0;
  }
}

int timer::getDelta(){
  return int(delta/6000);
}
