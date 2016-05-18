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
      lightsMode = keyPress;
      tvMode();
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
      Serial.println(uptime());
      break;
    // Shifted Second Row
    case 16:
      break;
    case 17:
      break;
    case 18:
      lightsMode = keyPress;
      flickerMode();
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
