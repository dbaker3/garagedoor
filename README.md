#Arduino Garagedoor Unlocker

* Built using Arduino Pro Mini 5v  
* You must initialize the Arduino's EEPROM with an initial passcode by uploading and running the `eeprom_init.ino` code before uploading this file.
* The servo connects to the door lock/unlock mechanism with a 4-bar linkage. A solenoid could be used as an alternative but would require an external power source as this cannot be provided by the Arduino.
* To activate the servo and unlock the door, enter the 4 digit code followed by # key. 
* Servo rotates moving linkage and the lock/unlock mechanism, unlocking door. 
* Door relocks after defined timeout. 
* Pushbutton located inside door unlocks door immediately without requiring code entry.
* During unlock, enter new 4 digit code followed by * key to change passcode stored in EEPROM.
* The Arduino enters low power mode after timeout. Interrupts set on keypad and inside button wake device from low power.
  
## Attributions
* Grallator - EEPROM, keypad input handling -  [grallator.wordpress.com](https://grallator.wordpress.com/2013/12/04/arduino-keypad-entry-system/)
* Nick Gammon - Low power and wake with keypad - [www.gammon.com.au](http://www.gammon.com.au/forum/?id=11497)
* madcoffee - Low power info & disabling power LED -  [www.home-automation-community.com](http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/)

## Schematic
![Schematic](https://raw.githubusercontent.com/dbaker3/garagedoor/master/garagedoor.png)
