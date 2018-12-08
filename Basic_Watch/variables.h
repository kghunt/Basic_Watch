// This will be set to 1 if usb is detected. You can manually set it also
// If you do it will stop the AVR from going to sleep and clobbering the USB connection.
int debug = 0;

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
int PreviousMenu = 0;

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

// Bluetooth stuff
#define BUFSIZE 128   // Size of the read buffer for incoming data
#define VERBOSE_MODE true // If set to 'true' enables debug output
#define BLUEFRUIT_SWUART_RXD_PIN       9    // Required for software serial!
#define BLUEFRUIT_SWUART_TXD_PIN       10   // Required for software serial!
#define BLUEFRUIT_UART_CTS_PIN         11   // Required for software serial!
#define BLUEFRUIT_UART_RTS_PIN -1 // Optional, set to -1 if unused

#ifdef Serial1    // this makes it not complain on compilation if there's no Serial1
  #define BLUEFRUIT_HWSERIAL_NAME      Serial1
#endif

#define BLUEFRUIT_UART_MODE_PIN 12 // Set to -1 if unused
#define BLUEFRUIT_SPI_CS               A2
#define BLUEFRUIT_SPI_IRQ              0
#define BLUEFRUIT_SPI_RST A1 // Optional but recommended, set to -1 if unused

#define BLUEFRUIT_SPI_SCK              13
#define BLUEFRUIT_SPI_MISO             12
#define BLUEFRUIT_SPI_MOSI 11

// USB Variables
//USB Power Detdction
bool usbConnected = false;
bool charging = false;
bool usbOneTimeEvent = true;

//ChargeLED(R) Vars
long int chargeledmillis = 0;
const int chargeledinterval = 50;
const int chargeledmaxvalue = 125;
const int chargeledminvalue = 10;
int chargeledfadevalue = chargeledminvalue; // !!Important to initialize the startvalue with minvalue!!
int chargeledfadesteps = 5;

#define CRG_STATE 5

// LEDs
#define LEDL        13
#define LEDR 6

