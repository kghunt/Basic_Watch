# Basic_Watch
A basic Arduino firmware for the WatchX

The eventual aim is to have a basic firmware that has. 
- Decent battery life.
- A basic text based menu.
- Bluetooth smart phone notifications.

It would also be nice if I can add (as long as battery is not affected too much):
- Wrist movement wake ups.
- Step counting.

The sketch uses some adafruit libraries that are not installed by default. If you are using the
latest Arduino IDE you can add them through the library manager. They are:

Adafruit_GFX
Adafruit_SSD1306
Adafruit_SleepyDog

It is important to note that when putting the watchx to sleep it kills USB comms that can only be fixed by resetting the watch.
This means uploading updates and such should be done by holding the reset button on the watch then pressing upload on the IDE.
Once the IDE says "uploading" release the reset switch and the sketch should upload.

If you are making lots of changes then comment out:
Watchdog.sleep(400);
Then the watch will not go to sleep until you uncomment it.

Battery Life
Sleep mode disabled: ~7-8 hours.
Sleep mode enabled:  33 hours and counting.

UPDATES by wbphelps - 07/08/2018
- general button logic cleanup: make button handling consistent, wakeup on all 3 buttons
- fix bug where values were being changed when entering time or tones menu.
- use 10x longer timeout when in menu 
- shorten keypress tones, use different tones for each button
- add reset function: press button1 & button 3 to reset watch
- Up/Down buttons wrap, don't update unless in Menu