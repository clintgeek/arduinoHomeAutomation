#include <RF24Network.h>
#include <RF24Network_config.h>
#include <Sync.h>

#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#include <Keypad.h>

// Configure Hardware
int const rOutPin = 10;
int const gOutPin = 9;
int const bOutPin = 6;

// RF24 Radio Setup
RF24 radio(7,8);
RF24Network network(radio);
const uint16_t this_node = 01;
const uint16_t other_node = 00;
struct payload_t {
  byte mode;
  byte param1;
  byte param2;
  byte param3;
};

// Keypad Setup
byte const ROWS = 4;
byte const COLS = 3;
char charKeys[ROWS][COLS] = {
  { 1, 2, 3 },
  { 4, 5, 6 },
  { 7, 8, 9 },
  { 10, 11, 12 }
};
byte rowPins[ROWS] = {A5, A4, A3, A2};
byte colPins[COLS] = {A1, A0, 2};
Keypad kpd = Keypad(makeKeymap(charKeys), rowPins, colPins, ROWS, COLS);

// Initialize Global Variables
int rVal;
int gVal;
int bVal;
int mode;
int shift;
int request;
bool abortNow;
int breatheSpeed = 30;

// Enable/Disable debug output
bool debug = false;

void setup() {
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  
  if (debug) {
    Serial.begin(19200);
    while (!Serial) {
      delay(5); // wait for serial port to connect. Needed for native USB port only
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
  network.update();
  if (network.available()) { networkInputProcessor(); }
  if (Serial.available()) { serialInputProcessor(); }
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

void networkInputProcessor() {
  while (network.available()) {
    RF24NetworkHeader header;
    payload_t payload;
    
    network.read(header,&payload,sizeof(payload));
    
    if (payload.mode) {   
      int mode = payload.mode;
      int param1 = payload.param1;
      int param2 = payload.param2;
      int param3 = payload.param3;
      
      debugPrinter("Mode: ", mode, 0);
      debugPrinter("Param1: ", param1, 0);
      debugPrinter("Param2: ", param2, 0);
      debugPrinter("Param3: ", param3, 0);

      paramManager(mode, param1, param2, param3);
    }
  }
}

void serialInputProcessor() {
  while (Serial.available()) {
    int i = 0;
    char serialRequest[13]; 
    
    while (true) {
      char inChar = Serial.read();
  
      if (isDigit(inChar)) {
        serialRequest[i] = inChar;
        i++;
      }
  
      if (inChar == '\n') {
        debugPrinter("serialRequest: ", serialRequest, 1);
        break;
      }
    }

    char modeArray[4] = {serialRequest[0], serialRequest[1], serialRequest[2]};
    char param1Array[4] = {serialRequest[3], serialRequest[4], serialRequest[5]};
    char param2Array[4] = {serialRequest[6], serialRequest[7], serialRequest[8]};
    char param3Array[4] = {serialRequest[9], serialRequest[10], serialRequest[11]};
  
    int mode = atoi(modeArray);
    int param1 = atoi(param1Array);
    int param2 = atoi(param2Array);
    int param3 = atoi(param3Array);
    
    paramManager(mode, param1, param2, param3);
  }
}

void paramManager(int mode, int param1, int param2, int param3) {
  if (mode == 001 && (param1 || param2 || param3)) {
    setRgb(param1, param2, param3);
  }

  inputManager(mode);
}

void inputManager(char input) {
  abortNow = true;
  request = input;

  debugPrinter("PROCESSED: ", request, 1);
  
  modeManager();
}

