// Basic_Watch by Karl Hunt
#include "variables.h"
#include "includes.h"
#include "clock.h"
#include "display.h"
#include "buttons.h"
#include "timer.h"

// Set to 1 while coding to make it easier to upload to watch. 
// When running normally set to 0 to enable powersaving (but you will need to use the reset metho to upload).
int debug = 1;

void setup() {
    // Set up the buttons as inputs
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  pinMode(Button3, INPUT_PULLUP);
  screeninit();
  ClockSetup();
  screenbrightness(brightness);
  watchface();
  previousMillis = millis();

}

void loop() {
  if (Menu == 0){
  watchface();
    }
//      else if (Menu == 1) {
//        MainMenu();
//      }
//      else if (Menu == 2) {
//        SetTimeMenu();
//      }
//      else if (Menu == 3) {
//        TonesMenu();
//      }
//      else if (Menu == 4){
//        AlarmMenu();
//      }
  timedevents();
  checkforpress();
    if (WatchState == 0 && debug == 0) {
    Watchdog.sleep(300);
  }
  

}
