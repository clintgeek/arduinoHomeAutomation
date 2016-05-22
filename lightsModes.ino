void powerOnSelfTest() {
  rgb(255, 0, 0);
  delay(1000);

  rgb(0, 255, 0);
  delay(1000);

  rgb(0, 0, 255);
  delay(1000);
  rgb(0, 0, 0);
}

void solidColorMode() {
  mode = request;
  while (!abortNow) {
    rgb(rVal, gVal, bVal);
    threadSafeDelay(50);
  }
}

void breatheMode() {
  mode = request;
  while (!abortNow) {
    breatheIn(primaryColor());
    if (abortNow) { break; }
    breatheOut(primaryColor());
  }
}

void allOff() {
  abortNow = true;
  mode = request = ' ';
  rgb(0, 0, 0); // Lights Off
}

void rgbFadeMode() {
  mode = request;
  int rgbColor[3];
  
  // Start off with red.
  rgbColor[0] = 255;
  rgbColor[1] = 0;
  rgbColor[2] = 0;

  while (!abortNow) {
    // Choose the colors to increment and decrement.
    for (int decColor = 0; decColor < 3; decColor += 1) {
      int incColor = decColor == 2 ? 0 : decColor + 1;
  
      if (abortNow) { break; }
  
      // cross-fade the two colors.
      for (int i = 0; i < 255; i += 1) {
        if (abortNow) { break; }
  
        rgbColor[decColor] -= 1;
        rgbColor[incColor] += 1;
  
        rgb(rgbColor[0], rgbColor[1], rgbColor[2]);
        threadSafeDelay(20);
      }
    }
  }
}

void pinkTurquoiseBreatheMode() {
  mode = request;
  while (!abortNow) {
    breathePink();
    if (abortNow) { break; }
    breatheTurquoise();
  }
}

void breathePink() {
  int brightness[3] = {0, 0, 0};

  for (int i = 0; i < 50; i++) {
    if (abortNow) { break; }

    brightness[0] += 4;
    brightness[1] += 0;
    brightness[2] += 1;

    rgb(brightness[0], brightness[1], brightness[2]);
    threadSafeDelay(60);
  }

  for (int i = 0; i < 50; i++) {
    if (abortNow) { break; }

    brightness[0] -= 4;
    brightness[1] -= 0;
    brightness[2] -= 1;

    rgb(brightness[0], brightness[1], brightness[2]);
    threadSafeDelay(60);
  }
}

void breatheTurquoise() {
  int brightness[3] = {0, 0, 0};

  for (int i = 0; i < 50; i++) {
    if (abortNow) { break; }

    brightness[0] += 0;
    brightness[1] += 4;
    brightness[2] += 4;

    rgb(brightness[0], brightness[1], brightness[2]);
    threadSafeDelay(60);
  }

  for (int i = 0; i < 50; i++) {
    if (abortNow) { break; }
    
    brightness[0] -= 0;
    brightness[1] -= 4;
    brightness[2] -= 4;

    rgb(brightness[0], brightness[1], brightness[2]);
    threadSafeDelay(60);
  }
}

void tvMode() {
  mode = request;
  while (!abortNow) {
    int rBrightness = random(0, 255);
    int gBrightness = random(0, 255);
    int bBrightness = random(0, 255);

    rgb(rBrightness, gBrightness, bBrightness);
    threadSafeDelay(250, 1750);
  }
}

void solidWhiteMode() {
  mode = request;
  while (!abortNow) {
    rgb(255, 197, 143);
    threadSafeDelay(50);
  }
}

void rgbBreatheMode() {
  mode = request;
  int color;
  while (!abortNow) {
    for (color = 0; color < 3; color++) {
      breatheIn(color);
      if (abortNow) { break; }
      breatheOut(color);
    }
  }
}

void sunriseMode(int color, int duration) {
  int rgbColor[3];
  long fadeInMillis = (duration * 60L * 1000L);
  int delayDuration = fadeInMillis / 255L;
  rgbColor[0] = 0;
  rgbColor[1] = 0;
  rgbColor[2] = 0;
  
  for (int brightness = 0; brightness <= 255; brightness++) {    
    if (abortNow) { break; }
   
    rgbColor[color] = brightness;
    rgb(rgbColor[0], rgbColor[1], rgbColor[2]);
    threadSafeDelay(delayDuration);
  }
 
  while (!abortNow) {
    threadSafeDelay(10);
  }
}

void sunsetMode(int color, int duration) {
  int rgbColor[3];
  long fadeInMillis = (duration * 60L * 1000L);
  int delayDuration = fadeInMillis / 255;
  rgbColor[0] = 0;
  rgbColor[1] = 0;
  rgbColor[2] = 0;

  for (int brightness = 255; brightness >= 0; brightness--) {
    if (abortNow) { break; }
    
    rgbColor[color] = brightness;
    rgb(rgbColor[0], rgbColor[1], rgbColor[2]);
    threadSafeDelay(delayDuration);
  }

  while (!abortNow) {
    threadSafeDelay(10);
  }
}

void flickerMode() {
  mode = request;
  while (!abortNow) {
    int colorIndex = random(0, 2);
    int colorBrightness = random(0, 255);

    writeSingleColor(colorIndex, colorBrightness);
    threadSafeDelay(250, 1500);
  }
}

