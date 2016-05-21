/* Garage Door Unlocker
*  David Baker 2016
*  
*  Using Arduino Pro Mini 5v
*  Need to initialize EEPROM with eeprom_init.ino code before uploading this file.
*  Servo connects to door lock mechanism with 4-bar linkage. Enter 4 digit code followed by # key. 
*  Correct code rotates servo, moving linkage and lock mechanism, unlocking door. Door relocks
*  after timeout. Pushbutton located inside door unlocks door immediately without code entry.
*  During unlock, enter new 4 digit code followed by * key to change code in EEPROM.
*  Enters low power mode after timeout. Interrupts on keypad and inside button wake from low power.
*  
*  Attributions:
*  Grallator - EEPROM, keypad input handling https://grallator.wordpress.com/2013/12/04/arduino-keypad-entry-system/
*  Nick Gammon - Low power and wake with keypad http://www.gammon.com.au/forum/?id=11497
*  madcoffee - Low power info & disabling power LED http://www.home-automation-community.com/arduino-low-power-how-to-run-atmega328p-for-a-year-on-coin-cell-battery/
*/

#include <EEPROM.h>
#include <Key.h>
#include <Keypad.h>
#include <Servo.h>
#include <avr/sleep.h>

// Setup Keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char buffer[4];
char passcode[4];

int keepUnlocked = 2000;
unsigned long unlockTime = 0;
bool unlocked = false;

// Inside button unlocks without code
int insideButtonPin = 10;
byte insideButtonState = 0;
unsigned long insideButtonPressTime = 0;
int insideButtonDebounceWait = 10; // milliseconds

Servo myservo;

int sleepAfterIdle = 5000;
unsigned long lastInputTime = 0;

// Clear interrupt vectors
EMPTY_INTERRUPT (PCINT0_vect)
EMPTY_INTERRUPT (PCINT1_vect)
EMPTY_INTERRUPT (PCINT2_vect)

void setup() {
  Serial.begin(9600);
  pinMode(insideButtonPin, INPUT);
  myservo.attach(9);
  myservo.write(90);
  loadEeprom();

  // pin change interrupt masks
  PCMSK2 |= _BV(PCINT21);  // pin 5
  PCMSK2 |= _BV(PCINT20);  // pin 4
  PCMSK2 |= _BV(PCINT19);  // pin 3
  PCMSK2 |= _BV(PCINT18);  // pin 2
  PCMSK0 |= _BV(PCINT2);   // pin 10
}

void loop() {
  sleepIfIdle();
  lockIfTime();
  handleInsideButtonPress();
  char key = keypad.getKey();
  if (key) {
    lastInputTime = millis();
    if (key != '#') {
      if (key == '*' && unlocked) { // Change passcode
        writeEeprom();
      }
      else {
        bufferInput(key);
      }
    }
    else {
      checkCode();
    }
    printBuffer();
  }
}

void unlock() {
  myservo.write(30);
  unlockTime = millis();
  unlocked = true;
  //delay(2000);
  //myservo.write(90);
  //unlocked = false;
}

void lockIfTime() {
  if ((unsigned long)(millis() - unlockTime) >= keepUnlocked) {
    myservo.write(90);
    unlocked = false;
  }
}

void bufferInput(char key) {
  for (int i = 0; i < 3; i++) {
    buffer[i] = buffer[i + 1];
  }
  buffer[3] = key;
}

void printBuffer() {
  for (int i = 0; i < 4; i++) {
    Serial.print(buffer[i]);
  }
  Serial.print("\n");
}

void checkCode() {
  if (memcmp(passcode, buffer, sizeof(passcode)) == 0) {
    Serial.println("Code Success");
    unlock();
  }
  else {

  }
  for (int i = 0; i < 4; i++) buffer[i] = 0;
}

void loadEeprom() {
  Serial.print("Current code: ");
  for (int i = 0; i < 4; i++) {
    passcode[i] = EEPROM.read(i);
    Serial.print(passcode[i]);
  }
  Serial.print("\n");
}

// Save last 4 input keys as new passcode
void writeEeprom() {
  for (int i = 0; i < 4; i++) {
    EEPROM.write(i, buffer[i]);
  }
  Serial.println("Code changed");
  loadEeprom();
}

void sleepIfIdle() {
  if ((unsigned long)(millis() - lastInputTime) >= sleepAfterIdle) {
    goToSleep();
  }
}

void goToSleep() {
  if (unlocked) return; // Don't sleep while unlocked
  Serial.println("Sleeping...");
  configPinsForKeyWake();
  delay(50); // overcome debounce in keypad lib
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  // Not sure what all actually needs disabling
  byte old_ADCSRA = ADCSRA;
  byte old_ADCSRB = ADCSRB;
  ADCSRA = 0; // Disable ADC
  ADCSRB = 0;
  byte old_ACSR = ACSR;
  ACSR = 0; // Disable analog comparator
  PRR = 0xFF; // Disable peripherals
  PCIFR  |= _BV (PCIF0) | _BV (PCIF1) | _BV (PCIF2);   // clear any outstanding interrupts
  PCICR  |= _BV (PCIE0) | _BV (PCIE1) | _BV (PCIE2);   // enable pin change interrupts
  MCUCR = _BV (BODS) | _BV (BODSE); // Disable brownout detection
  MCUCR = _BV (BODS);
  sleep_cpu ();
  // CPU starts here on wake:
  sleep_disable(); // Cancel sleep as a precaution
  PCICR = 0;  // cancel pin change interrupts
  PRR = 0;    // enable modules again
  ADCSRA = old_ADCSRA; // re-enable ADC conversion
  ADCSRB = old_ADCSRB;
  ACSR = old_ACSR; // re-enable analog comparator
  configPinsForKeypadUse();
  Serial.println("Awake!");
}

// Configure keypad pins as per keypad library
void configPinsForKeypadUse () {
  for (byte i = 0; i < 3; i++) {
    pinMode (colPins [i], OUTPUT);
    digitalWrite (colPins [i], HIGH);
  }
  for (byte i = 0; i < 4; i++) {
    pinMode (rowPins [i], INPUT);
    digitalWrite (rowPins [i], HIGH);
  }
}

// Configure keypad pins to pull low to activate interrupt
void configPinsForKeyWake() {
  for (byte i = 0; i < 3; i++) {
    pinMode (colPins [i], OUTPUT);
    digitalWrite (colPins [i], LOW);
  }
  for (byte i = 0; i < 4; i++) {
    pinMode (rowPins [i], INPUT);
    digitalWrite (rowPins [i], HIGH);  // rows high (pull-up)
  }
}

// Check if inside unlock button pressed and unlock
void handleInsideButtonPress() {
  if (digitalRead(insideButtonPin) == LOW && insideButtonState == 0) {
    if ((unsigned long)(millis() - insideButtonPressTime >= insideButtonDebounceWait)) {
      lastInputTime = millis();
      insideButtonState = 1;
      Serial.println("Inside button pressed");
      unlock();
    }
  } // keep from registering multiple presses while held down
  else if (digitalRead(insideButtonPin) == LOW && insideButtonState == 1){
    insideButtonState = 1;
  }
  else {
    insideButtonState = 0;
  }
}

