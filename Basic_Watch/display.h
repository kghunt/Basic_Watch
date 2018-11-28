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

void watchface() {
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
  oled.setCursor(0,7);
  oled.print("Batt:");


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

  oled.print(percent);
  
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

}
