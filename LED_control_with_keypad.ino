#include <Keypad.h>

// Configure Hardware
int const rOutPin = 11;
int const gOutPin = 10;
int const bOutPin = 9;
int const fOutPin = 6;

// Keypad Setup
byte const ROWS = 4;
byte const COLS = 3;
char charKeys[ROWS][COLS] = {
  { 1, 2, 3 },
  { 4, 5, 6 },
  { 7, 8, 9 },
  { 10, 11, 12 }
};
byte rowPins[ROWS] = {13, 12, 8, 7};
byte colPins[COLS] = {5, 4, 3};
Keypad kpd = Keypad(makeKeymap(charKeys), rowPins, colPins, ROWS, COLS);

// Initialize Variables
int rVal = 0;
int gVal = 0;
int bVal = 0;
int shift;
bool abortNow;
int lightsMode;
int keyPadRequest;
int changeSpeed = 20;
char serialRequest[13];
bool isSerialInputComplete = false;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    delay(1); // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("\nArduino Booting...");

  powerOnSelfTest();

  Serial.println("\nArduino Ready!");
  Serial.println();
}

void loop() {
  keyPressManager();
  changeManager();
}

int keyPadMonitor() {
  if (kpd.getKeys())
  {
    for (int i = 0; i < LIST_MAX; i++) // Scan the whole key list.
    {
      if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
      {
        switch (kpd.key[i].kstate) // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
        {
          case PRESSED:
            shift = 1;
            Serial.print("PRESSED: ");
            Serial.print(kpd.key[i].kcode + 1);
            Serial.println( );
            break;
          case HOLD:
            shift = 13;
            Serial.print("HELD: ");
            Serial.print(kpd.key[i].kcode + shift);
            Serial.println( );
            break;
          case RELEASED:
            Serial.print("RELEASED: ");
            Serial.print(kpd.key[i].kcode + shift);
            Serial.println( );
            if ((kpd.key[i].kcode + 1) > 0)
            {
              return (kpd.key[i].kcode + shift);
            }
            break;
          case IDLE:
            break;
        }
      }
    }
  }
}

void serialEvent() {
  int i = 0;
  
  if (Serial.available()) {
    while (true) {
      char inChar = Serial.read();
  
      if (isDigit(inChar)) {
        serialRequest[i] = inChar;
        i++;
      }
  
      if (inChar == '\n') {
        isSerialInputComplete = true;
        break;
      }
    }
  }
}

int serialInputHandler() {
  if (isSerialInputComplete) {

    Serial.print("SERIAL: ");
    Serial.print(serialRequest);
    Serial.println( );

    char modeArray[4] = {serialRequest[0], serialRequest[1], serialRequest[2]};
    char var1Array[4] = {serialRequest[3], serialRequest[4], serialRequest[5]};
    char var2Array[4] = {serialRequest[6], serialRequest[7], serialRequest[8]};
    char var3Array[4] = {serialRequest[9], serialRequest[10], serialRequest[11]};

    int mode = atoi(modeArray);
    int var1 = atoi(var1Array);
    int var2 = atoi(var2Array);
    int var3 = atoi(var3Array);

    if (var1 || var2 || var3) {
      setColorVals(var1, var2, var3);
    }

    memset(serialRequest, 0, sizeof(serialRequest));
    isSerialInputComplete = false;

    return mode;
  }
}

bool keyPressManager() {
  int manualKeyPress = keyPadMonitor();
  int serialKeyPress = serialInputHandler();
  int keyPress;
  
  if (manualKeyPress) {
    keyPress = manualKeyPress;
    Serial.print("PROCESSING: keypad");
    Serial.println();
  } else if (serialKeyPress) {
    keyPress = serialKeyPress;
    Serial.print("PROCESSING: serial");
    Serial.println();
  } else {
    keyPress = false;
  }

  if (keyPress) {
    Serial.print("PROCESSED: ");
    Serial.print(keyPress);
    Serial.println( );
    Serial.println( );

    if (isModeKeyPress(keyPress)) { // catch mode changes
      keyPadRequest = keyPress;
      changeManager();
    } else {
      modeManager(keyPress);
    }
    return true;
  } else {
    return false;
  }
}

char modeKeyPresses[8] = {1, 2, 4, 5, 13, 14, 15, 100};

bool isModeKeyPress(char keyPress) {
  for (int i = 0; i < sizeof(modeKeyPresses); i++) {
    if (modeKeyPresses[i] == keyPress) {
      return true;
    }
  }
  return false;
}

bool changeDetected() {
  bool requestAbort = (keyPadRequest != lightsMode);
  bool serialChange = (Serial.peek() != -1);

  return (abortNow || requestAbort || keyPressManager() || serialChange);
}

void changeManager() {
  if (keyPadRequest != lightsMode) {
    Serial.println("MODE CHANGE DETECTED:");
    Serial.print("keyPadRequest: ");
    Serial.println(keyPadRequest);
    Serial.print("lightsMode: ");
    Serial.println(lightsMode);
    Serial.println();

    modeManager(keyPadRequest);
  } else {
    modeManager(lightsMode);
  }
}

void rgb(int r, int g, int b) {
  analogWrite(rOutPin, r);
  analogWrite(gOutPin, g);
  analogWrite(bOutPin, b);
}

void setColorVals(int r, int g, int b) {
  rVal = r;
  gVal = g;
  bVal = b;
}

void powerOnSelfTest() {
  rgb(255, 0, 0);
  delay(1000);

  rgb(0, 255, 0);
  delay(1000);

  rgb(0, 0, 255);
  delay(1000);
  rgb(0, 0, 0);
}

void adjustColor(char color, char direction) {
  switch (color) {
    case 'r':
      rVal = adjustBrightness(rVal, direction);
      break;
    case 'g':
      gVal = adjustBrightness(gVal, direction);
      break;
    case 'b':
      bVal = adjustBrightness(bVal, direction);
      break;
  }
}

int adjustBrightness(int brightness, char direction) {
  if (direction == 'u') {
    if (brightness <= 245) {
      brightness = brightness + 10;
    } else {
      brightness = 255;
    }
  } else {
    if (brightness >= 10) {
      brightness = brightness - 10;
    } else {
      brightness = 0;
    }
  }
  return brightness;
}

int primaryColor() {
  int primaryColor;
  if (rVal > gVal && rVal > bVal) {
    primaryColor = 0;
  }
  else if (gVal > rVal && gVal > bVal) {
    primaryColor = 1;
  }
  else primaryColor = 2;

  return primaryColor;
}
