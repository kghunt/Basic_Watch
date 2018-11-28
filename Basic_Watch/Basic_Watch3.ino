// Basic_Watch by Karl Hunt
#include "variables.h"
#include "includes.h"
#include "clock.h"
#include "display.h"
#include "buttons.h"
#include "timer.h"

void setup() {
  
  screeninit();
  ClockSetup();
  screenbrightness(1);
  watchface();

}

void loop() {
  watchface();
  timedevents();
  
  

}
