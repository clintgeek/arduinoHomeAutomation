#include <Keypad.h>

// Configure Hardware
int const rOutPin = 11;
int const gOutPin = 10;
int const bOutPin = 9;

// Keypad Setup
byte const ROWS = 4;
byte const COLS = 3;
char charKeys[ROWS][COLS] = {
  { 1, 2, 3 },
  { 4, 5, 6 },
  { 7, 8, 9 },
  { 10, 11, 12 }
};
byte rowPins[ROWS] = {12, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3};
Keypad kpd = Keypad(makeKeymap(charKeys), rowPins, colPins, ROWS, COLS);

// Initialize Global Variables
int rVal;
int gVal;
int bVal;
int mode;
int shift;
int request;
bool abortNow;

// Enable/Disable debug output
bool debug = false;

void setup() {
  if (debug) {
    Serial.begin(57600);
    while (!Serial) {
      delay(1); // wait for serial port to connect. Needed for native USB port only
    }
  }
  
  debugPrinter("Arduino Booting...", 0);

  randomSeed(analogRead(0));
  powerOnSelfTest();

  debugPrinter("Arduino Ready!", 1);
}

void loop() {
  inputWatcher();
}

void inputWatcher() {
  if (kpd.getKeys()) { keypadInputProcessor(); }
}

void keypadInputProcessor() {  
  for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
  {
    if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
    {
      switch (kpd.key[i].kstate) // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
      {
        case PRESSED:
          shift = 1;
          debugPrinter("PRESSED: ", kpd.key[i].kcode + shift, 0);
          break;
        case HOLD:
          shift = 13;
          debugPrinter("HELD: ", kpd.key[i].kcode + shift, 0);
          break;
        case RELEASED:
          debugPrinter("RELEASED: ", kpd.key[i].kcode + shift, 0);
          if ((kpd.key[i].kcode + 1) > 0)
          {
            inputManager(kpd.key[i].kcode + shift);
          }
          break;
        case IDLE:
          break;
        default:
          break;
      }
    }
  }
}

void inputManager(char input) {
  abortNow = true;
  request = input;

  debugPrinter("PROCESSED: ", request, 1);
  
  modeManager();
}

