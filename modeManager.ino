void modeManager(int request) {
  switch (request) {
    // First Row
    case 1:
      mode = request;
      solidColorMode();
      break;
    case 2:
      mode = request;
      breatheMode();
      break;
    case 3:
      mode = request;
      allOff();
      break;
    // Second Row
    case 4:
      mode = request;
      rgbFadeMode();
      break;
    case 5:
      mode = request;
      pinkTurquoiseBreatheMode();
      break;
    case 6:
      mode = request;
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
      mode = request;
      solidWhiteMode();
      break;
    case 14:
      mode = request;
      rgbBreatheMode();
      break;
    case 15:
      break;
    // Shifted Second Row
    case 16:
      mode = request;
      sunriseMode(2, 5);
      break;
    case 17:
      mode = request;
      sunsetMode(0, 5);
      break;
    case 18:
      mode = request;
      danceMode();
      break;
    // Shifted Third Row
    case 19:
      adjustColor('r', 'x');
      break;
    case 20:
      adjustColor('g', 'x');
      break;
    case 21:
      adjustColor('b', 'x');
      break;
    // Shifted Fourth Row
    case 22:
      adjustColor('r', 'n');
      break;
    case 23:
      adjustColor('g', 'n');
      break;
    case 24:
      adjustColor('b', 'n');
      break;
    // Other Modes
    case 25:
      ambiLightMode();
      break;
  }
}
