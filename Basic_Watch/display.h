// Stuff relating to the display in here

// pin definitions
#define CS_PIN  A5
#define RST_PIN A4
#define DC_PIN  A3

SSD1306AsciiSpi oled;

// A variable for the padded time
char datebuffer[6];

// Battery variables
static byte bPin = A11;
static byte battEn = 4;
// Battery range 3.4 - 4.22v?
// Voltage divider resistor values.
float R1 = 10000;
float R2 = 10000;
float vDivider;
float voltage;
int percent;

void wake() {
  oled.ssd1306WriteCmd(SSD1306_DISPLAYON);
  WatchState = 1;
}

// This fucntion puts the screen to sleep
void sleep() {
  oled.ssd1306WriteCmd(SSD1306_DISPLAYOFF);
  WatchState = 0;
}

void screeninit() {
  oled.begin(&Adafruit128x64, CS_PIN, DC_PIN, RST_PIN);
  oled.clear();
}

void screenbrightness(int level) {
  if (level == 1) {
    oled.setContrast(51);
  }
  else if (level == 2) {
    oled.setContrast(102);
  }
  if (level == 3) {
    oled.setContrast(153);
  }
  if (level == 4) {
    oled.setContrast(204);
  }
  if (level == 5) {
    oled.setContrast(255);
  }
}
void cleardisplay(){
  oled.clear();
}

void watchface() {
  Menu = 0;
  DateTime now = rtc.now(); // Store the current time. // Delcared globally as its used a lot. It broke when this was global

  oled.set1X(); // Set default text size
  oled.setFont(System5x7);

  // Display date
  oled.setCursor(40,0);
  oled.print(now.day(), DEC);
  oled.print("-");
  oled.print(now.month(), DEC);
  oled.print("-");
  oled.print(now.year(), DEC);

  

  // col(pixels),row(8 pixel rows)



    // Battery measurements and calculations
  vDivider = (R2 / (R1 + R2));
  digitalWrite(battEn, HIGH);
  delay(50);
  voltage = analogRead(bPin);
  voltage = (voltage / 1024) * 3.35;
  voltage = voltage / vDivider;
  delay(50);
  digitalWrite(battEn, LOW);
  percent = (voltage - 3.4) / 0.008;

  if (percent > 100) {
    percent = 100;
  }

  
  // lcdnums12x16 has a bug where when there is a space it drops the remaining characters trying a different font
  // all the fixednums and lcd nums and the disapearing minute issue.
  oled.setFont(fixed_bold10x15); // This is the best font I could find that fills the screen nicely Maybe make our own?

  oled.set2X(); // Double font size
  oled.setCursor(10, 2); // Centre time

  // Flash the colon for seconds. This stores the time in a variable and changes the presence of the colon based on even and odd seconds.
  if (now.second() % 2) sprintf(datebuffer, "%02u:%02u", now.hour(), now.minute());
  else sprintf(datebuffer, "%02u %02u", now.hour(), now.minute());

  // Display the time
  oled.print(datebuffer);

    oled.set1X();
    oled.setFont(System5x7);
    oled.setCursor(0,7);
  oled.print("Batt:");
  oled.print(percent);
  oled.print("%");

  if (usbConnected){
    oled.setCursor(80,7);
    oled.print("USB");
    
    if (charging){
      oled.setCursor(105,7);
      oled.print("Chrg");
    } else {
        oled.setCursor(105,7);
        oled.print("Full");
      }

    if(usbOneTimeEvent){
      //Todo: Code here executed just for once when the usb is connected
      usbOneTimeEvent = false;       
    }
   
  } else{
      if(!usbOneTimeEvent){
        oled.setCursor(80,7);
        oled.clearToEOL();  
      }
    } 
    
}

  // This function accepts a list of strings and draws a text menu with them
void MakeMenu(char* title, char* data[], int length, int option){
  oled.clear();
  oled.set1X();
  oled.setFont(System5x7);

   oled.setCursor(0,0);
   oled.print("sel");
   oled.setCursor(117,0);
   oled.print("up");
    
  int titlespace = (21 - strlen(title)) / 2; // Calculate title spacing

   //   Pad title so it is centred
  for (int i = 0; i < titlespace; i++){
    oled.print(" ");
  }

  // Print title
  oled.print(title); // print title
  oled.setCursor(0,1);

  // Print menu items
  for (int i = 0; i < length ; i++){
    if (i == option){
      oled.print(">");
    }
    else{
       oled.print(" ");
    }
   
    oled.println(data[i]);

    
    
  }

    oled.setCursor(117,7);
    oled.print("dn");



    delay(100);
}

void MainMenu(){
  char b[15];
  sprintf(b,"Brightness: %d", brightness);
    char* items[] {"Exit", "Set Time", "Tones Menu", b, "Alarm"};
  Menu = 1;
  MaxMenu = 5;
  MinMenu = 1;
  MakeMenu("Main Menu", items, MaxMenu, MenuOption);
}

void SetTimeMenu(){
  //SetDateTimeVar();
  sprintf(y, "Year  :%d", year);
  sprintf(M, "Month :%02u", month);
  sprintf(d, "Day   :%02u", day);
  sprintf(h, "Hour  :%02u", hour);
  sprintf(m, "Minute:%02u", minute);
  char* items[] {y, M, d, h, m, "Save", "Cancel"};
  MaxMenu = 7;
  MinMenu = 1;
  Menu = 2;
  MakeMenu("Set Time", items, MaxMenu, MenuOption);
 
}

void TonesMenu(){
  char KT[18];
  if (KeyPressTone == 0){
    sprintf(KT, "Key Tones    :OFF");
  }
  else{
    sprintf(KT, "Key Tones    :ON");
  }
    char HT[18];
  if (HourlyTone == 0){
    sprintf(HT, "Hourly Tones :OFF");
  }
  else{
    sprintf(HT, "Hourly Tones :ON");
  }
    char* items[] {"Exit", KT, HT};
  MaxMenu = 3;
  MinMenu = 1;
  Menu = 3;
  MakeMenu("Tone Settings", items, MaxMenu, MenuOption);
}

void AlarmMenu(){
  char AE[18];
  if (alarmenabled == 1){
    sprintf(AE, "Alarm Enabled:Yes" );
      }
  else{
    sprintf(AE, "Alarm Enabled:No" );
  }
  char AM[10];
  sprintf(AM, "Minute:%d",alarmmin);
  char AH[8];
  sprintf(AH, "Hour:%d", alarmhour);
  char* items[] {AE, AH, AM, "Exit"};
  Menu = 4;
  MinMenu = 1;
  MaxMenu = 4;
  MakeMenu("Alarm", items, MaxMenu, MenuOption);
  

}

void bluflash(){
  tone(buzzer, 2000, 50);
  delay(800);  
  noTone(buzzer);
  ble.println("AT+BLEUARTTX=OK");
  if (WatchState == 0){
  wake();
  }
  oled.clear();
  oled.println("Notification");
  oled.println(ble.buffer);
  delay(5000);
  oled.clear();
  
}

// Bluetooth loop commands
void bluloop(){
 ble.println("AT+BLEUARTRX");
 ble.readline();
  if (strcmp(ble.buffer, "OK") == 0) {
    // no data
    return;
  }
  // Some data was found, its in the buffer
  bluflash();
  ble.waitForOK();
} 


