#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "RTClib.h"
// Slow the cpu clock to save power with the following module
//#include "cpu.h"

#include <Adafruit_SleepyDog.h>

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
const long interval = 5000;


// Which menu option is currently selected
int MenuOption = 1;

// Which Menu is currently on the display
int Menu = 0;
int MaxMenu;
int MinMenu;

// Watch state
// 0 = Screen off
// 1 = Screen on
int WatchState = 0;

// time to limit multiple presses.
int debounce = 300;

// Strings to store manual date and time values
char date[11];
char Time[9];


int year;
int month;
int day;
int hour;
int minute;
int second;

// Disable hourly beeps (to be added to a menu soon)
int HourlyTones = 0;
// Disable button press tones (to be added to a menu soon)
int KeyPressTones = 0;

void setup()   {
  //Serial.begin(9600);
  
  // Battery saving stuffs I will work on this later
  //#define BLUEFRUIT_SPI_CS               A2
  //#define BLUEFRUIT_SPI_IRQ              0
  //#define BLUEFRUIT_SPI_RST              A1    // Optional but recommended, set to -1 if unused
  //Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
  //ble.sendCommandCheckOK(F( "AT+HWMODELED=DISABLE" )); 
  //delay(100); ble.sendCommandCheckOK(F( "AT+UARTFLOW=OFF" )); 
  //delay(100); ble.sendCommandCheckOK(F( "AT+BLEPOWERLEVEL=-12" )); 
  //delay(100); ble.sendCommandCheckOK(F( "AT+BLEMIDIEN=OFF" )); 
  //delay(100); ble.sendCommandCheckOK(F( "AT+GAPSTOPADV" ));
  
  rtc.begin();

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC);
  // init done

  // Clear the buffer.
  display.clearDisplay();

  // Set the watch time (when uploading sketch)
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  SetDateTimeVar();

  // Set up the buttons as inputs
  pinMode(Button1, INPUT_PULLUP);
  pinMode(Button2, INPUT_PULLUP);
  pinMode(Button3, INPUT_PULLUP);

  // For voltage calulation
  vDivider = (R2 / (R1 + R2));

  //wake();
  Watchface();
}


void loop() {
  DateTime now = rtc.now();
  

  
  // Double beep on the hour <-- todo make this occur once so we don't need the delay on the end
  if (now.minute() == 00 && now.second() == 00){
    if (HourlyTones == 1){
   tone(buzzer,2000,200);
   delay(300);
   tone(buzzer,2000,200);
   delay(800);
   noTone(buzzer);
    }
  }
  // Single beep on the half hour <-- todo make this occur once so we don't need the delay on the end
  if (now.minute() == 30 && now.second() == 00){
    if (HourlyTones == 1){
   tone(buzzer,2000,200);
   delay(1200);
   noTone(buzzer);
    }
  }
  
  
  
  // Sleep display if timer runs out
  currentMillis = millis();
  //Serial.println(currentMillis);
  if (currentMillis - previousMillis >= interval) {
    // Turn display off because the timer ran out
    sleep();

  }


  if (digitalRead(Button1) == 0){
    if (KeyPressTones == 1){
    tone(buzzer,3000,100);
    }
    if (WatchState == 0){
      wake();
      Watchface();
    }
    else if (WatchState == 1 && Menu == 0){
      wake();
      MainMenu();
      delay(debounce);
    }
    else if (WatchState == 1 && Menu == 1){
      wake();
      ExecuteAction(MenuOption);
      delay(debounce);
    }
    else if (WatchState == 1 && Menu == 2){
      wake();
      ExecuteAction(MenuOption);
      delay(debounce);
    }
  }
  if (digitalRead(Button2) == 0 && WatchState == 0){
   wake(); 
  }
  if (digitalRead(Button2) == 0 && Menu != 0 && MenuOption > MinMenu){
    if (KeyPressTones == 1){
    tone(buzzer,3000,100);
    }
    wake();
    MenuOption --;
    if (Menu == 1){
    MainMenu();
    }
    if (Menu == 2){
    SetTimeMenu();
    }
    delay(debounce);
  }

  if (digitalRead(Button3) == 0 && Menu != 0 && MenuOption < MaxMenu){
    if (KeyPressTones == 1){
    tone(buzzer,3000,100);
    }
    wake();
    MenuOption ++;
    if (Menu == 1){
    MainMenu();
    }
    if (Menu == 2){
    SetTimeMenu();
    }
    delay(debounce);
  }

// put chip to sleep for 400 milliseconds
if (WatchState == 0){
Watchdog.sleep(400);
}

}


void wake() {
  display.ssd1306_command(SSD1306_DISPLAYON);
  WatchState = 1;
  //Serial.println("Wake");
  previousMillis = currentMillis;


}

void sleep() {
  //Serial.println("Sleep");
  display.ssd1306_command(SSD1306_DISPLAYOFF);
  WatchState = 0;

}


void Watchface(){
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
  if (percent < 0){
     percent = 0;
    
  }
  if (percent > 100){
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
  display.print(F("Batt:"));
  display.print(percent);
  display.print(F("%"));
  display.display();

  // Show date
  display.setCursor(40, 0);
  display.print(now.day(), DEC);
  display.print(F("-"));
  display.print(now.month(), DEC);
  display.print(F("-"));
  display.print(now.year(), DEC);
  display.display();
  
  delay(debounce);


}


void MainMenu(){
  //Serial.println("Main Menu");
  Menu = 1;
  MaxMenu = 3;
  MinMenu = 1;
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(F("sel"));
  display.setCursor(40, 0);
  display.print(F("Main Menu"));
  display.setCursor(5, 12);
  display.print(F("Exit"));
  display.setCursor(5, 22);
  display.print(F("Set Time"));
  display.setCursor(5, 32);
  display.print(F("Tones"));
  display.setCursor(115, 0);
  display.print(F("up"));
  display.setCursor(115, 57);
  display.print(F("dn"));
  if (MenuOption == 1){
    display.setCursor(0, 12);
    display.print(F(">"));
  }
    if (MenuOption == 2){
    display.setCursor(0, 22);
    display.print(F(">"));
  }
    if (MenuOption == 3){
    display.setCursor(0, 32);
    display.print(F(">"));
  }
  
  display.display();
  delay(debounce);
}

void Exit(){
 Watchface();
 Menu = 0;
}

void ExecuteAction(int option){
  if (Menu == 1){
  if (option == 1){
   Exit(); 
  }
  if (option == 2){
    MenuOption = 1;
    SetTimeMenu();
  }
  }

  if (Menu == 2){
      if (option == 1){
   year++;
   if (year > 2020){
    year = 2018;
   }
   SetTimeMenu(); 
  }
      if (option == 2){
   month++;
   if (month > 12){
    month = 1;
   }
   SetTimeMenu(); 
  }
      if (option == 3){
   day++;
   // this needs to be smarter as some months have less days
   if (day > 31){
    day = 1;
   }
   SetTimeMenu(); 
  }
      if (option == 4){
   hour++;
   if (hour > 23){
    hour = 0;
   }
   SetTimeMenu(); 
  }
      if (option == 5){
   minute++;
   if (minute > 59){
    minute = 0;
   }
   SetTimeMenu(); 
  }
      if (option == 6){
   year++;
   SetTime();
   MenuOption = 1;
   MainMenu();
  }
      if (option == 7){
  MenuOption = 1;
  SetDateTimeVar();
  MainMenu();
  }
  
  }


}
void SetTimeMenu(){
  //delay(debounce);
  MaxMenu = 7;
  MinMenu = 1;
  Menu = 2;

  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(40, 0);
  display.print(F("Set Time"));
  display.setCursor(0, 0);
  display.print(F("sel"));
  display.setCursor(115, 0);
  display.print(F("up"));
  display.setCursor(115, 57);
  display.print(F("dn"));

  display.setCursor(5, 10);
  display.print(F("Year  :"));
  display.setCursor(50, 10);
  display.print(year);
  display.setCursor(5, 18);
  display.print(F("Month :"));
  display.setCursor(50, 18);
  display.print(month);
  display.setCursor(5, 26);
  display.print(F("Day   :"));
  display.setCursor(50, 26);
  display.print(day);
  display.setCursor(5, 34);
  display.print(F("Hour  :"));
  display.setCursor(50, 34);
  display.print(hour);
  display.setCursor(5, 42);
  display.print(F("Minute:"));
  display.setCursor(50, 42);
  display.print(minute);
  display.setCursor(5, 50);
  display.print(F("Save"));
  display.setCursor(80, 50);
  display.print(F("Cancel"));

  if (MenuOption == 1){
    display.setCursor(0, 10);
    display.print(F(">"));
  }
    if (MenuOption == 2){
    display.setCursor(0, 18);
    display.print(F(">"));
  }
    if (MenuOption == 3){
    display.setCursor(0, 26);
    display.print(F(">"));
  }
    if (MenuOption == 4){
    display.setCursor(0, 34);
    display.print(F(">"));
  }
    if (MenuOption == 5){
    display.setCursor(0, 42);
    display.print(F(">"));
  }
    if (MenuOption == 6){
    display.setCursor(0, 50);
    display.print(F(">"));
  }
    if (MenuOption == 7){
    display.setCursor(75, 50);
    display.print(F(">"));
  }
  display.display();

}

void SetTime(){
rtc.adjust(DateTime(year,month,day,hour,minute,0));
  
}

  
void SetDateTimeVar(){
  DateTime noW = rtc.now();
  year = noW.year();
  month = noW.month();
  day = noW.day();
  hour = noW.hour();
  minute = noW.minute();
  second = noW.second();
}


