// button logic file

void silencealarm(){
  if (alarm == 1){
    alarmstop = 1;
  }
if (alarm == 0){
  alarmstop = 0;
}
}
// This function is the exit menu option
void Exit() {
  cleardisplay();
  watchface();
  Menu = 0;
  MenuOption = 0;
}

// Execute a selected menu option
void ExecuteAction(int option) {

  // Menu 1 Actions
  if (Menu == 1) {
    if (option == 0) {
      Exit();
    }
    else if (option == 1) {
      MenuOption = 0;
      SetDateTimeVar();
      SetTimeMenu();
    }
    else if (option == 2) {
      MenuOption = 0;
      TonesMenu();
    }
    // Menu 4 actions
  else if (option == 3){
     if (brightness < 5){
      brightness ++;
     }
     else{
      brightness = 1;
     }
    screenbrightness(brightness);
    MainMenu();
  }
  else if (option == 4){
    MenuOption = 0;
    AlarmMenu();
  }
  }

  // Menu 2 Actions
  else if (Menu == 2) {
    if (option == 0) {
      year++;
      if (year > 2020) {
        year = 2018;
      }
      SetTimeMenu();
    }
    else if (option == 1) {
      month++;
      if (month > 12) {
        month = 1;
      }
      SetTimeMenu();
    }
    else if (option == 2) {
      day++;
      // this needs to be smarter as some months have less days
      if (day > 31) {
        day = 1;
      }
      SetTimeMenu();
    }
    else if (option == 3) {
      hour++;
      if (hour > 23) {
        hour = 0;
      }
      SetTimeMenu();
    }
    else if (option == 4) {
      minute++;
      if (minute > 59) {
        minute = 0;
      }
      SetTimeMenu();
    }
    else if (option == 5) {
      SetTime();
      MenuOption = 0;
      cleardisplay();
      MainMenu();
    }
    else if (option == 6) {
      MenuOption = 0;
      SetDateTimeVar();
      cleardisplay();
      MainMenu();
    }

  }

  // Menu 3 actions
  else if (Menu == 3) {
    if (option == 0) {
      MenuOption = 0;
      MainMenu();
    }
    else if (option == 1) {
      if (KeyPressTone == 0) {
        KeyPressTone = KeyPressToneTime;  // enable key press tones
      }
      else {
        KeyPressTone = 0;  // off
      }
      TonesMenu();
    }
    else if (option == 2) {
      if (HourlyTone == 0) {
        HourlyTone = HourlyToneTime;  // enable hourly tones
        tone(buzzer, 2000, 100);
      }
      else {
        HourlyTone = 0;
      }
      cleardisplay();
      TonesMenu();
    }

  }
  // Menu 4 actions
  else if (Menu == 4) {
    if (option == 0){
      if (alarmenabled == 0){
      alarmenabled = 1;
      }
      else{
        alarmenabled = 0;
      }
      AlarmMenu();
    }
    else if (option == 1){
      if (alarmhour < 23){
        alarmhour ++;
      }
      else {
        alarmhour = 0;
      }
      AlarmMenu();
    }
        else if (option == 2){
      if (alarmmin < 59){
        alarmmin ++;
      }
      else {
        alarmmin = 0;
      }
      AlarmMenu();
    }
    else if (option ==3){
      option = 0;
      MainMenu();
    }
  }

}




void checkforpress(){
 // manage button presses.
  if (digitalRead(Button1) == 0 && millis() - previousMillis >= debounce) {  // Button 0
    silencealarm();
    previousMillis = millis();
    if (KeyPressTone > 0) {
      tone(buzzer, NOTE_C7, KeyPressToneTime);
    }
//    if (digitalRead(Button3) == 0) {
//      resetFunc();  // hold buttons 1 & 3 to reset watch
//    }
    if (WatchState == 0) {
      wake();
      watchface();
    }
    else if (WatchState == 1 && Menu == 0) {
      MainMenu();
    }
    else if (WatchState == 1 && Menu > 0) {
      ExecuteAction(MenuOption);
    }
    //delay(debounce);
  }

  if (digitalRead(Button2) == 0 && millis() - previousMillis >= debounce) {  // UP
    silencealarm();
    previousMillis = millis();
    if (KeyPressTone > 0) {
      tone(buzzer, NOTE_D7, KeyPressToneTime);
    }
    if (WatchState == 1 && Menu == 0){
             skipprev();
    }
    if (WatchState == 0) {
      wake();
      if (Menu == 0) {
 
        watchface();
      }
    }
    else if (Menu > 0) {
      if (MenuOption > MinMenu - 1) {
        MenuOption --;
      }
      else {
        MenuOption = MaxMenu - 1;  // Wrap
      }
      // not needed?
      if (Menu == 1) {
        MainMenu();
      }
      else if (Menu == 2) {
        SetTimeMenu();
      }
      else if (Menu == 3) {
        TonesMenu();
      }
      else if (Menu == 4){
        AlarmMenu();
      }
    }
   
  }  // button 2

  if (digitalRead(Button3) == 0  && millis() - previousMillis >= debounce) {  // DOWN
    silencealarm();
    previousMillis = millis();
    if (KeyPressTone > 0) {
      tone(buzzer, NOTE_B6, KeyPressToneTime);
    }
    if (WatchState == 1 && Menu == 0){
       skipnext();
    }
    if (WatchState == 0) {
      wake();
      if (Menu == 0) {
       
        watchface();
      }
    }
    else if (Menu > 0) {
      if (MenuOption < MaxMenu -1 ) {
        MenuOption ++;
      }
      else {
        MenuOption = 0;  // wrap around
      }
      if (Menu == 1) {
        MainMenu();
     }
      else if (Menu == 2) {
       SetTimeMenu();
     }
     else if (Menu == 3) {
       TonesMenu();
     }
         else if (Menu == 4){
          AlarmMenu();
         }
    }

  }  // button 3
  
}

