# Basic_Watch
A basic Arduino firmware for the WatchX

Some code for USB detection and breathing LED was taken from Tap Clock
https://github.com/venice1200/TapClock

The eventual aim is to have a basic firmware that has. 
- Decent battery life.
- A basic text based menu.
- Bluetooth smart phone notifications.
- Easy to read modifiable code. Especially for newcomers.

It would also be nice if I can add (as long as battery is not affected too much):
- Wrist movement wake ups.
- Step counting.

The sketch uses some adafruit libraries that are not installed by default. If you are using the
latest Arduino IDE you can add them through the library manager. They are:

Adafruit_GFX (no longer used)
Adafruit_SSD1306 (no longer used)
Grieman Ascii SSD1306 Library.
Adafruit_SleepyDog

It is important to note that when putting the watchx to sleep it kills USB comms that can only be fixed by resetting the watch.
This means uploading updates and such should be done by holding the reset button on the watch then pressing upload on the IDE.
Once the IDE says "uploading" release the reset switch and the sketch should upload.

If you are making lots of changes then comment out:
change the debug to 1 on the main file.
Then the watch will not go to sleep until you change it back.

Battery Life:
Sleep mode disabled: ~7-8 hours.
Sleep mode enabled:  33 hours and counting, I broke my screen during testing but I was up at over 50 hours with 40% battery remaining.

UPDATES by wbphelps - 07/08/2018
- general button logic cleanup: make button handling consistent, wakeup on all 3 buttons
- fix bug where values were being changed when entering time or tones menu.
- use 10x longer timeout when in menu 
- shorten keypress tones, use different tones for each button
- add reset function: press button1 & button 3 to reset watch
- Up/Down buttons wrap, don't update unless in Menu

Updates by kghunt
- Almost complete re-write
- Uses Ascii text only library saving around 15-20% storage
- Added Brightness
- Added Alarm (not fully implemented)
