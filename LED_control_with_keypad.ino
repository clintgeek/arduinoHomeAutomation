#include <Adafruit_SleepyDog.h>
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
Keypad kpd = Keypad( makeKeymap(charKeys), rowPins, colPins, ROWS, COLS);

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

  pinMode(fOutPin, OUTPUT);
  digitalWrite(fOutPin, LOW);

  powerOnSelfTest();

  Watchdog.enable(4000);

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
  while (Serial.available()) {
    char inChar = Serial.read();

    if (isDigit(inChar)) {
      serialRequest[i] = inChar;
      i++;
    }

    if (inChar == '\n') {
      isSerialInputComplete = true;
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

    memset(serialRequest,0,sizeof(serialRequest));
    isSerialInputComplete = false;

    return mode;
  }
}

bool keyPressManager() {
  int manualKeyPress = keyPadMonitor();
  int serialKeyPress = serialInputHandler();

  Watchdog.reset();

  int keyPress = manualKeyPress ? manualKeyPress : serialKeyPress;

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
    modeManager(keyPadRequest);
  } else {
    modeManager(lightsMode);
  }
}

void modeManager(int keyPress) {
  switch (keyPress) {
    // First Row
    case 100:
    case 1:
      lightsMode = keyPress;
      rgb(rVal, gVal, bVal); // Solid Color
      break;
    case 2:
      lightsMode = keyPress;
      breatheMode(false); // Single Color Breathe
      break;
    case 3:
      lightsMode = ' ';
      keyPadRequest = ' ';
      rgb(0, 0, 0); // Lights Off
      break;
    // Second Row
    case 4:
      lightsMode = keyPress;
      fadeMode(); // RGB Fade
      break;
    case 5:
      lightsMode = keyPress;
      pinkTurquoiseBreathe();
      break;
    case 6:
      break;
    // Third Row
    case 7:
      adjustColor('r', 'u');
      break;
    case 8:
      adjustColor('g', 'u');
      break;
    case 9:
      adjustColor('b', 'u');
      break;
    // Fourth Row
    case 10:
      adjustColor('r', 'd');
      break;
    case 11:
      adjustColor('g', 'd');
      break;
    case 12:
      adjustColor('b', 'd');
      break;
    // Shifted First Row
    case 13:
      lightsMode = keyPress;
      rgb(255, 197, 143); // White Light
      break;
    case 14:
      lightsMode = keyPress;
      breatheMode(true); // RGB Breathe
      break;
    case 15:
      lightsMode = ' ';
      keyPadRequest = ' ';
      rgb(0, 0, 0); // Lights Off
      break;
    // Shifted Second Row
    case 16:
      break;
    case 17:
      break;
    case 18:
      break;
    // Shifted Third Row
    case 19:
      rVal = 255;
      break;
    case 20:
      gVal = 255;
      break;
    case 21:
      bVal = 255;
      break;
    // Shifted Fourth Row
    case 22:
      rVal = 0;
      break;
    case 23:
      gVal = 0;
      break;
    case 24:
      bVal = 0;
      break;
  }

  abortNow = false;
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
    }
  } else {
    if (brightness >= 10) {
      brightness = brightness - 10;
    }
  }
  return brightness;
}

void fadeMode() {
  unsigned int rgbColor[3];

  // Start off with red.
  rgbColor[0] = 255;
  rgbColor[1] = 0;
  rgbColor[2] = 0;

  // Choose the colors to increment and decrement.
  for (int decColor = 0; decColor < 3; decColor += 1) {
    int incColor = decColor == 2 ? 0 : decColor + 1;

    if (changeDetected()) {
      abortNow = true;
      break;
    }

    // cross-fade the two colors.
    for (int i = 0; i < 255; i += 1) {
      if (changeDetected()) {
        abortNow = true;
        break;
      }

      rgbColor[decColor] -= 1;
      rgbColor[incColor] += 1;

      rgb(rgbColor[0], rgbColor[1], rgbColor[2]);
      delay(changeSpeed);
    }
  }
}

void breatheMode(bool rgbBreathe) {
  int color;

  if (rgbBreathe) {
    for (color = 0; color < 3; color++) {
      if (changeDetected()) {
        abortNow = true;
        break;
      }

      breatheIn(color);
      if (changeDetected()) {
        abortNow = true;
        break;
      }
      breatheOut(color);
    }
  } else {
    color = primaryColor();
    if (!changeDetected()) {
      breatheIn(color);
    }
    if (!changeDetected()) {
      breatheOut(color);
    }
  }
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

void breatheIn(int color) {
  unsigned int rgbColor[3];

  rgbColor[0] = 0;
  rgbColor[1] = 0;
  rgbColor[2] = 0;

  for (int brightness = 0; brightness <= 255; brightness++) {
    if (changeDetected()) {
      abortNow = true;
      break;
    }

    rgbColor[color] = brightness;

    rgb(rgbColor[0], rgbColor[1], rgbColor[2]);
    delay(changeSpeed);
  }
}

void breatheOut(int color) {
  unsigned int rgbColor[3];

  rgbColor[0] = 0;
  rgbColor[1] = 0;
  rgbColor[2] = 0;

  rgbColor[color] = 255;

  for (int brightness = 255; brightness >= 1; brightness--) {
    if (changeDetected()) {
      abortNow = true;
      break;
    }

    rgbColor[color] = brightness;

    rgb(rgbColor[0], rgbColor[1], rgbColor[2]);
    delay(changeSpeed);
  }
}

void pinkTurquoiseBreathe() {
  breathePink();
  breatheTurquoise();
}

void breathePink() {
  int brightness[3] = {0, 0, 0};

  for (int i = 0; i < 50; i++) {
    if (changeDetected()) {
      abortNow = true;
      break;
    }

    brightness[0] += 4;
    brightness[1] += 0;
    brightness[2] += 1;

    rgb(brightness[0], brightness[1], brightness[2]);
    delay(50);
  }

  for (int i = 0; i < 50; i++) {
    if (changeDetected()) {
      abortNow = true;
      break;
    }

    brightness[0] -= 4;
    brightness[1] -= 0;
    brightness[2] -= 1;

    rgb(brightness[0], brightness[1], brightness[2]);
    delay(50);
  }
}

void breatheTurquoise() {
  int brightness[3] = {0, 0, 0};

  for (int i = 0; i < 50; i++) {
    if (changeDetected()) {
      abortNow = true;
      break;
    }

    brightness[0] += 0;
    brightness[1] += 4;
    brightness[2] += 4;

    rgb(brightness[0], brightness[1], brightness[2]);
    delay(50);
  }

  for (int i = 0; i < 50; i++) {
    if (changeDetected()) {
      abortNow = true;
      break;
    }

    brightness[0] -= 0;
    brightness[1] -= 4;
    brightness[2] -= 4;

    rgb(brightness[0], brightness[1], brightness[2]);
    delay(50);
  }
}

