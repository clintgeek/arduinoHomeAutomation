#include <Keypad.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>

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
  unsigned char request;
  unsigned char red;
  unsigned char green;
  unsigned char blue;
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

// Enable/Disable debug output
bool debug = true;

void setup() {
  if (debug) {
    Serial.begin(57600);
    while (!Serial) {
      delay(1); // wait for serial port to connect. Needed for native USB port only
    }
  }
  
  debugPrinter("Arduino Booting...", 0);

  SPI.begin();
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);

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
  RF24NetworkHeader header;
  payload_t payload;
  
  network.read(header,&payload,sizeof(payload));
  
  if (payload.request) { 
    unsigned char networkRequest = payload.request - 48;
    unsigned char networkRed = payload.red;
    unsigned char networkGreen = payload.green;
    unsigned char networkBlue = payload.blue;

    debugPrinter("rf24Network request: ", networkRequest, 0);
    debugPrinter("rf24Network red: ", networkRed, 0);
    debugPrinter("rf24Network green: ", networkGreen, 0);
    debugPrinter("rf24Networku blue: ", networkBlue, 0);

    if (networkRequest == 1) { setRgb(networkRed, networkGreen, networkBlue); }
    inputManager(networkRequest); 
  }
}

void inputManager(char input) {
  abortNow = true;
  request = input;

  debugPrinter("PROCESSED: ", request, 1);
  
  modeManager();
}

