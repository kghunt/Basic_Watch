//#include <Adafruit_ATParser.h>
//#include <Adafruit_BLE.h>
//#include <Adafruit_BLEBattery.h>
//#include <Adafruit_BLEEddystone.h>
//#include <Adafruit_BLEGatt.h>
//#include <Adafruit_BLEMIDI.h>
//#include <Adafruit_BluefruitLE_SPI.h>
//#include <Adafruit_BluefruitLE_UART.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"
// Slow the cpu clock to save power with the following module
//#include "cpu.h"

#include <Adafruit_SleepyDog.h>

// Updated 07/08/2018 - wbphelps
// general button logic cleanup: make button handling consistent, wakeup on all 3 buttons
// fix bug where values were being changed when entering time or tones menu.
// use 10x longer timeout when in menu
// shorten keypress tones, use different tones for each button
// add reset function: press button1 & button 3 to reset watch
// Up/Down buttons wrap around, only update if menu active

#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349

RTC_DS3231 rtc;

// Uncomment this block to use hardware SPI
#define OLED_DC     A3
#define OLED_CS     A5
#define OLED_RESET  A4
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);

#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

#define SSD1306_LCDHEIGHT 64

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
// A variable for the padded time
char datebuffer[6];

// Pin definitions
#define Button1 8
#define Button2 11
#define Button3 10
#define BATLVL 5
static byte bPin = A11;
static byte battEn = 4;
#define buzzer 9

// Battery range 3.4 - 4.22v?
// Voltage divider resistor values.
float R1 = 10000;
float R2 = 10000;
float vDivider;
float voltage;
int percent;
unsigned long currentMillis;
unsigned long previousMillis = 0;
const long interval = 5000;  // go back to sleep after 5 seconds

// Tracking watch state
// I plan to use an integer to track what state the watch is in
// this should make it more effecient so we can check the state
// before running routines that do not need to be run.
// It can also be used to see what menu or item the watch is currently on.
// Initially these will be as follows.
// 0 = Watchface
// 1 = Menu 1
// 2 = MM option 1
// 3 = MM option 2
// etc
int MenuOption = 1;
int Menu = 0;
int MaxMenu;
int MinMenu;

// Watch state
// 0 = Screen off
// 1 = Screen on
// 3 = Sleep?
int WatchState = 0;

// time to limit multiple presses.
int debounce = 300;

// Variables to store manual date and time values
int year;
int month;
int day;
int hour;
int minute;
int second;

// Button press tones 
int KeyPressTone = 15;  // enabled
const int KeyPressToneTime = 15;  // short chirp 10 ms
// Hourly beeps
int HourlyTone = 0;  // disabled
const int HourlyToneTime = 100;  // 100 ms

// Bluetooth module pins
#define BLUEFRUIT_SPI_CS               A2
#define BLUEFRUIT_SPI_IRQ              0
#define BLUEFRUIT_SPI_RST              A1    // Optional but recommended, set to -1 if unused

void setup() {

  // Set up the buttons as inputs
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  pinMode(Button3, INPUT_PULLUP);

  if (KeyPressTone > 0){
  tone(buzzer,1000,KeyPressTone);
  }
  //Serial.begin(9600);

  // Battery saving stuffs. This should turn off the bluetooth module putting it to sleep.
//  Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
//  ble.sendCommandCheckOK(F( "AT+HWMODELED=DISABLE" ));
//  delay(100); ble.sendCommandCheckOK(F( "AT+UARTFLOW=OFF" ));
//  delay(100); ble.sendCommandCheckOK(F( "AT+BLEPOWERLEVEL=-12" ));
//  delay(100); ble.sendCommandCheckOK(F( "AT+BLEMIDIEN=OFF" ));
//  delay(100); ble.sendCommandCheckOK(F( "AT+GAPSTOPADV" ));

  rtc.begin();

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done

  // Clear the buffer.
  display.clearDisplay();

  // Set the watch time (when uploading sketch)
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //rtc.adjust(DateTime(2018,07,02,23,23,0));

  SetDateTimeVar();

  // For voltage calulation
  vDivider = (R2 / (R1 + R2));

  //wake();
  previousMillis = millis(); 
  Watchface();
  delay(3000);
  sleep(); 
}

void(* resetFunc) (void) = 0;//declare reset function at address 0

// The main loop Primarily used to monitor button presses and exceute actions.
void loop() {
  DateTime now = rtc.now();

  if (HourlyTone > 0) {  // if hourly tones are enabled, check the time
  // Double beep on the hour <-- todo make this occur once so we don't need the delay on the end
    if (now.minute() == 00 && now.second() == 00) {
      tone(buzzer, 2000, HourlyTone);
      delay(300);
      tone(buzzer, 2000, HourlyTone);
      delay(800);
      noTone(buzzer);
    }
  // Single beep on the half hour <-- todo make this occur once so we don't need the delay on the end
    if (now.minute() == 30 && now.second() == 00) {
      tone(buzzer, 2000, HourlyTone);
      delay(800);  // wbp - was 1200???
      noTone(buzzer);
    }
  }


  // Sleep display if timer runs out
//  currentMillis = millis();
  //Serial.println(currentMillis);
  int int1 = interval;
  if (Menu > 0) {
    int1 = interval*10;
  }
  if (millis() - previousMillis >= int1) {
    sleep();  // Turn display off because the timer ran out
  }

  // manage button presses.
  if (digitalRead(Button1) == 0) {  // Button 0
    previousMillis = millis();
    if (KeyPressTone > 0) {
      tone(buzzer, NOTE_C7, KeyPressTone);
    }
    if (digitalRead(Button3) == 0) {
      resetFunc();  // hold buttons 1 & 3 to reset watch
    }
    if (WatchState == 0) {
      wake();
      Watchface();
    }
    else if (WatchState == 1 && Menu == 0) {
      MainMenu();
    }
    else if (WatchState == 1 && Menu > 0) {
      ExecuteAction(MenuOption);
    }
  delay(debounce);
  }
  
  if (digitalRead(Button2) == 0) {  // UP 
    previousMillis = millis();
    if (KeyPressTone > 0) {
      tone(buzzer, NOTE_D7, KeyPressTone);
    }
    if (WatchState == 0) {
      wake();
      if (Menu == 0) {
        Watchface();
      }
    }
    else if (Menu > 0) {
      if (MenuOption > MinMenu) {
        MenuOption --;
        }
      else {
        MenuOption = MaxMenu;  // Wrap
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
    }
  delay(debounce);
  }  // button 2

  if (digitalRead(Button3) == 0) {  // DOWN
    previousMillis = millis();
    if (KeyPressTone > 0) {
      tone(buzzer, NOTE_B6, KeyPressTone);
    }
    if (WatchState == 0) {
      wake();
      if (Menu == 0) {
        Watchface();
      }
    }
    else if (Menu > 0) {
      if (MenuOption < MaxMenu) {
        MenuOption ++;
      }
      else {
        MenuOption = 1;  // wrap around
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
    }
  delay(debounce);
  }  // button 3

  // put chip to sleep for 400 milliseconds
  if (WatchState == 0) {
    Watchdog.sleep(400);
  }


}

// This function wakes the screen up
void wake() {
  display.ssd1306_command(SSD1306_DISPLAYON);
  WatchState = 1;
  //Serial.println("Wake");
//  previousMillis = millis();
}

// This fucntion puts the screen to sleep
void sleep() {
  //Serial.println("Sleep");
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  WatchState = 0;
}

// This function displays the main watchface
// Todo seperate non watchface calcs into a seperate function eg. battery calcs
void Watchface() {
  //Serial.print("Watch face");
  display.clearDisplay();
  // Battery measurements and calculations
  digitalWrite(battEn, HIGH);
  delay(50);
  voltage = analogRead(bPin);
  voltage = (voltage / 1024) * 3.35;
  voltage = voltage / vDivider;
  delay(50);
  digitalWrite(battEn, LOW);
  percent = (voltage - 3.4) / 0.008;
  if (percent < 0) {
    percent = 0;

  }
  if (percent > 100) {
    percent = 100;
  }

  // Screen stuff
  display.setTextSize(4);
  display.setTextColor(WHITE);

  // Show Time
  display.setCursor(4, 15);
  DateTime now = rtc.now();
  sprintf(datebuffer, "%02u:%02u", now.hour(), now.minute());
  display.clearDisplay();
  display.setCursor(4, 15);
  display.print(datebuffer);

  // Show battery percentage
  display.setTextSize(1);
  display.setCursor(1, 54);
  display.print("Batt:");
  display.print(percent);
  display.print("%");
  display.display();

  // Show date
  display.setCursor(40, 0);
  display.print(now.day(), DEC);
  display.print("-");
  display.print(now.month(), DEC);
  display.print("-");
  display.print(now.year(), DEC);
  display.display();

//  delay(debounce);
}

// This function displays the main menu
void MainMenu() {
  //Serial.println("Main Menu");
  Menu = 1;
  MaxMenu = 3;
  MinMenu = 1;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("sel");
  display.setCursor(40, 0);
  display.print("Main Menu");
  display.setCursor(5, 12);
  display.print("Exit");
  display.setCursor(5, 22);
  display.print("Set Time");
  display.setCursor(5, 32);
  display.print("Tones");
  display.setCursor(115, 0);
  display.print("up");
  display.setCursor(115, 57);
  display.print("dn");
  if (MenuOption == 1) {
    display.setCursor(0, 12);
    display.print(">");
  }
  if (MenuOption == 2) {
    display.setCursor(0, 22);
    display.print(">");
  }
  if (MenuOption == 3) {
    display.setCursor(0, 32);
    display.print(">");
  }

  display.display();
  delay(debounce);
}

// This function is the exit menu option
void Exit() {
  Watchface();
  Menu = 0;
}

// Execute a selected menu option
void ExecuteAction(int option) {

  // Menu 1 Actions
  if (Menu == 1) {
    if (option == 1) {
      Exit();
    }
    else if (option == 2) {
      MenuOption = 1;
      SetTimeMenu();
    }
    else if (option == 3) {
      MenuOption = 1;
      TonesMenu();
    }
  }

  // Menu 2 Actions
  else if (Menu == 2) {
    if (option == 1) {
      year++;
      if (year > 2020) {
        year = 2018;
      }
      SetTimeMenu();
    }
    else if (option == 2) {
      month++;
      if (month > 12) {
        month = 1;
      }
      SetTimeMenu();
    }
    else if (option == 3) {
      day++;
      // this needs to be smarter as some months have less days
      if (day > 31) {
        day = 1;
      }
      SetTimeMenu();
    }
    else if (option == 4) {
      hour++;
      if (hour > 23) {
        hour = 0;
      }
      SetTimeMenu();
    }
    else if (option == 5) {
      minute++;
      if (minute > 59) {
        minute = 0;
      }
      SetTimeMenu();
    }
    else if (option == 6) {
      SetTime();
      MenuOption = 1;
      MainMenu();
    }
    else if (option == 7) {
      MenuOption = 1;
      SetDateTimeVar();
      MainMenu();
    }

  }

// Menu 3 actions
  else if (Menu == 3){
    if (option == 1) {
      MenuOption = 1;
      MainMenu();
    }
    else if (option == 2) {
      if (KeyPressTone == 0) {
        KeyPressTone = KeyPressToneTime;  // enable key press tones
      }
      else {
        KeyPressTone = 0;  // off
      }
      TonesMenu();
    }
    else if (option == 3) {
      if (HourlyTone == 0) {
        HourlyTone = HourlyToneTime;  // enable hourly tones
        tone(buzzer,2000,100);
      }
      else {
        HourlyTone = 0;
      }
      TonesMenu();
    }
    
  }

}

// This function displays the menu for setting the time and date
void SetTimeMenu() {

  MaxMenu = 7;
  MinMenu = 1;
  Menu = 2;

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(40, 0);
  display.print("Set Time");
  display.setCursor(0, 0);
  display.print("sel");
  display.setCursor(115, 0);
  display.print("up");
  display.setCursor(115, 57);
  display.print("dn");

  display.setCursor(5, 10);
  display.print("Year  :");
  display.setCursor(50, 10);
  display.print(year);
  display.setCursor(5, 18);
  display.print("Month :");
  display.setCursor(50, 18);
  display.print(month);
  display.setCursor(5, 26);
  display.print("Day   :");
  display.setCursor(50, 26);
  display.print(day);
  display.setCursor(5, 34);
  display.print("Hour  :");
  display.setCursor(50, 34);
  display.print(hour);
  display.setCursor(5, 42);
  display.print("Minute:");
  display.setCursor(50, 42);
  display.print(minute);
  display.setCursor(5, 50);
  display.print("Save");
  display.setCursor(80, 50);
  display.print("Cancel");

  if (MenuOption == 1) {
    display.setCursor(0, 10);
    display.print(">");
  }
  else if (MenuOption == 2) {
    display.setCursor(0, 18);
    display.print(">");
  }
  else if (MenuOption == 3) {
    display.setCursor(0, 26);
    display.print(">");
  }
  else if (MenuOption == 4) {
    display.setCursor(0, 34);
    display.print(">");
  }
  else if (MenuOption == 5) {
    display.setCursor(0, 42);
    display.print(">");
  }
  else if (MenuOption == 6) {
    display.setCursor(0, 50);
    display.print(">");
  }
  else if (MenuOption == 7) {
    display.setCursor(75, 50);
    display.print(">");
  }
  display.display();

}

// This function actually sets the time
void SetTime() {
  rtc.adjust(DateTime(year, month, day, hour, minute, 0));
}

// This fucntion prepares some variables to display when setting the time
void SetDateTimeVar() {
  DateTime noW = rtc.now();
  year = noW.year();
  month = noW.month();
  day = noW.day();
  hour = noW.hour();
  minute = noW.minute();
  second = noW.second();
}

// Menu for setting the tones
void TonesMenu(){
  MaxMenu = 3;
  MinMenu = 1;
  Menu = 3;

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(28, 0);
  display.print("Tone Settings");
  display.setCursor(0, 0);
  display.print("sel");
  display.setCursor(115, 0);
  display.print("up");
  display.setCursor(115, 57);
  display.print("dn");
 
  display.setCursor(5, 10);
  display.print("Exit");
  
  display.setCursor(5, 18);
  display.print("Key Tones   :");
  display.setCursor(85, 18);
  if (KeyPressTone > 0){
      display.print("On");
  }
  else{
    display.print("Off");
  }

  display.setCursor(5, 26);
  display.print("Hourly Tones:");
  display.setCursor(85, 26);
  
  if (HourlyTone > 0) {
    display.print("On");
  }
  else{
    display.print("Off");
  }


  if (MenuOption == 1) {
    display.setCursor(0, 10);
    display.print(">");
  }
  else if (MenuOption == 2) {
    display.setCursor(0, 18);
    display.print(">");
  }
    else if (MenuOption == 3) {
    display.setCursor(0, 26);
    display.print(">");
  }

  display.display();
}

