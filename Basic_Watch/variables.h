int WatchState = 1;

// buzzer pin
#define buzzer 9

// Current Menu
int Menu = 0;

// screen timeout interval
unsigned long interval = 5000;
unsigned long int1 = 5000;
unsigned long currentMillis;
unsigned long previousMillis = millis();

// tones
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349

// buttons
#define Button1 8
#define Button2 11
#define Button3 10

// time to limit multiple presses.
int debounce = 250;

// Button press tones
int KeyPressTone = 0;  // enabled
const int KeyPressToneTime = 15;  // short chirp 10 ms
// Hourly beeps
int HourlyTone = 0;  // disabled
const int HourlyToneTime = 100;  // 100 ms

// Menu Variables
int MinMenu;
int MaxMenu;
int MenuOption;

// For the time menu
char y[12];
char M[10];
char d[10];
char h[10];
char m[10];

// Brightness setting
int brightness = 1;

// Alarm variables
int alarmenabled = 0;
int alarm = 0;
int alarmstop = 0;
int alarmhour = 00;
int alarmmin = 00;
