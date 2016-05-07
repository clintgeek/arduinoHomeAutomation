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
