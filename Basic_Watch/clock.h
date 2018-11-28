// This file deals with RTC and time related stuff

RTC_DS3231 rtc;

// Variables to store manual date and time values
int year;
int month;
int day;
int hour;
int minute;
int second;
int lastday;

//DateTime now = rtc.now(); // Had to go back to local as this crashed the MCU???

void ClockSetup(){
  // Initialize RTC
  rtc.begin();
  // Set the watch time (when uploading sketch)
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //rtc.adjust(DateTime(2018,07,02,23,23,0));
}

// This function prepares some variables to display when setting the time
void SetDateTimeVar() {
  DateTime now = rtc.now();
  year = now.year();
  month = now.month();
  day = now.day();
  hour = now.hour();
  minute = now.minute();
  second = now.second();
}

// This function actually sets the time
void SetTime() {
  rtc.adjust(DateTime(year, month, day, hour, minute, 0));
}

// This function wakes the screen up







