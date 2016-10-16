#include <RF24Network.h>
#include <RF24Network_config.h>
#include <Sync.h>

#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>

#include "DHT.h"

#include <Keypad.h>

/////////////////////////////
// NODE LIST
/////////////////////////////
# define RASPBERRY_PI 00
# define HOME_OFFICE 01
# define MASTER_BEDROOM 02
# define LIVING_ROOM 03
# define GARAGE_DOOR 04
/////////////////////////////

/////////////////////////////
// PIN LIST
/////////////////////////////
//  0: RX
//  1: TX
//  2: 
// *3:
//  4: KEY(OPT)
// *5: DHT22
// *6: BLUE
//  7: rf24(2)
//  8: rf24(6)
// *9: RED
//*10: GREEN
//*11: rf24(7)
// 12: rf24(4)
// 13: rf24(3)
// A0: KEY(R0-G)
// A1: KEY(R1-O)
// A2: KEY(R2-OW)
// A3: KEY(C0-Br)
// A4: TV(+)
// A5: KEY(C1-BrW)
// A6: KEY(C2-BW)
// A7: KEY(C3-B)
// Ground: rf24(1), DHT(4)
// 5V: DHT(1), Vin
// 3v3: rf24(5)
// *PWM
/////////////////////////////

// NODE SPECIFIC CONFIGURATION
const uint16_t this_node = MASTER_BEDROOM;
const bool has_dht = true;
#define DHTTYPE DHT22
const bool has_key = false;
const bool has_tv = false;
const bool debug = true;

// Configure RGB Strip
int const gOutPin = 10;
int const rOutPin = 9;
int const bOutPin = 6;

// Configure TV Sensor
#define TV_SENSOR A4
unsigned long prevTvCheckMillis = 0;
unsigned long delayTvCheckMillis = 3000;

// Configure DHT Sensor
#define DHTPIN 5
DHT dht(DHTPIN, DHTTYPE);
unsigned long prevDhtCheckMillis = 0;
unsigned long delayDhtCheckMillis = 30000;

// Keypad Setup
byte const ROWS = 4;
byte const COLS = 3;
char charKeys[ROWS][COLS] = {
  { 1, 2, 3 },
  { 4, 5, 6 },
  { 7, 8, 9 },
  { 10, 11, 12 }
};
byte rowPins[ROWS] = {A7, A6, A5, A3};
byte colPins[COLS] = {A2, A1, A0};
Keypad kpd = Keypad(makeKeymap(charKeys), rowPins, colPins, ROWS, COLS);

// RF24 Radio Setup
RF24 radio(7,8);
RF24Network network(radio);
const uint16_t pi_node = 00;
struct payload_command {
  byte mode;
  byte param1;
  byte param2;
  byte param3;
};
struct payload_environment {
  int sensor_type;
  int param1;
  int param2;
  int param3;
};

// Initialize Global Variables
int rVal;
int gVal;
int bVal;
int mode;
int shift;
int request;
bool abortNow;
bool prevTvStatus;
int breatheSpeed = 40;
int previousTemperature;
int previousHumidity;
unsigned long currentMillis;

void setup() {
  radio.begin();
  network.begin(/*channel*/ 90, /*node address*/ this_node);
  dht.begin();

  if (debug) {
    Serial.begin(19200);
    while (!Serial) {
      delay(5); // wait for serial port to connect. Needed for native USB port only
    }
    Serial.flush();
    delay(1000);
  }

  debugPrinter("Arduino Booting...", 0);

  debugPrinter("My node id is: ", this_node, 0);
  debugPrinter("Have DHT: ", has_dht, 0);
  debugPrinter("Have TV: ", has_tv, 0);
  debugPrinter("Have Key: ", has_key, 0);

  powerOnSelfTest();

  debugPrinter("Arduino Ready!", 1);
}

void loop() {
  inputWatcher();
}

void inputWatcher() {
  currentMillis = millis();
  if (has_dht) { dhtMonitor(); }
  if (has_tv) { checkTvStatus(); }
  
  network.update();
  if (network.available()) { networkInputProcessor(); }
  if (Serial.available()) { serialInputProcessor(); }
  if (has_key && kpd.getKeys()) { keypadInputProcessor(); }
}

void dhtMonitor() {
  if (currentMillis - prevDhtCheckMillis >= delayDhtCheckMillis) {
    prevDhtCheckMillis = currentMillis;
    int f = dht.readTemperature(true);
    int h = dht.readHumidity();

    // Check if any reads failed and exit early (to try again).
    if (isnan(f) || isnan(h)) {
      debugPrinter("Failed to read from DHT sensor!", 1);
      return;
    }

    if (f != previousTemperature || h != previousHumidity) {
      debugPrinter("Temperature(F): ", f, 0);
      debugPrinter("Humidity(%): ", h, 1);

      previousTemperature = f;
      previousHumidity = h;
      sendSensorData(1, f, h);
    } else {
      debugPrinter("Temp/Humidity unchanged", 1);
    }
  }
}

void checkTvStatus() {
  if (currentMillis - prevTvCheckMillis >= delayTvCheckMillis) {
    int tv_sensor_level = analogRead(TV_SENSOR);

    if (tv_sensor_level > 700) { debugPrinter("TV Sensor: ", tv_sensor_level, 1); }

    prevTvCheckMillis = currentMillis;
    bool currentTvStatus = (tv_sensor_level == 1023);
    if (prevTvStatus == currentTvStatus) {
      return;
    } else if (prevTvStatus == true) {
      prevTvStatus = false;
      debugPrinter("TV Off", 0);
      sendSensorData(2, 0);
      inputManager(3);
    } else {
      prevTvStatus = true;
      debugPrinter("TV On", 0);
      sendSensorData(2, 1);
      inputManager(13);
    }
  }
}

void networkInputProcessor() {
  while (network.available()) {
    RF24NetworkHeader header;
    payload_command payload;

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
          inputManager(kpd.key[i].kcode + shift);
          break;
        case IDLE:
          break;
        default:
          break;
      }
    }
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

