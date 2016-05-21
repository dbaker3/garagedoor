#Arduino Garagedoor Unlocker

Using Arduino Pro Mini 5v  
Need to initialize EEPROM with eeprom_init.ino code before uploading this file.
Servo connects to door lock mechanism with 4-bar linkage. Enter 4 digit code followed by # key. 
Correct code rotates servo, moving linkage and lock mechanism, unlocking door. Door relocks
after timeout. Pushbutton located inside door unlocks door immediately without code entry.
During unlock, enter new 4 digit code followed by * key to change code in EEPROM.
Enters low power mode after timeout. Interrupts on keypad and inside button wake from low power.
  
Attributions:
* Grallator - EEPROM, keypad input handling -  [grallator.wordpress.com](https://grallator.wordpress.com/2013/12/04/arduino-keypad-entry-system/)
* Nick Gammon - Low power and wake with keypad - [www.gammon.com.au](http://www.gammon.com.au/forum/?id=11497)
* madcoffee - Low power info & disabling power LED -  [www.home-automation-community.com](http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/)
