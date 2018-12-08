// Basic_Watch by Karl Hunt
#include "variables.h"
#include "includes.h"
#include "bluetooth.h"
#include "clock.h"
#include "display.h"
#include "buttons.h"
#include "timer.h"



void setup() {
    // Set up the buttons as inputs
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  pinMode(Button3, INPUT_PULLUP);
  pinMode(CRG_STATE, INPUT_PULLUP);
  pinMode(LEDL, OUTPUT);
  pinMode(LEDR, OUTPUT);
  digitalWrite(LEDL, LOW);
  digitalWrite(LEDR, LOW);
  screeninit();
  ClockSetup();
  screenbrightness(brightness);
  watchface();
  previousMillis = millis();
  bluetoothinit();
  USBCON|=(1<<OTGPADE); 


  
}

void loop() {
  usbstatecheck();  
  if (Menu == 0){
  watchface();
    }

  timedevents();
  checkforpress();
  bluloop();
    if (WatchState == 0 && debug == 0 && !usbConnected) {
    Watchdog.sleep(300);
    USBDevice.attach();
    
  }
  

}
