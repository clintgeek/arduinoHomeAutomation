void modeManager() {
  abortNow = false;
  
  switch (request) {
    // First Row
    case 100:
    case 1:
      solidColorMode();
      break;
    case 2:
      breatheMode();
      break;
    case 3:
      allOff();
      break;
    // Second Row
    case 4:
      rgbFadeMode();
      break;
    case 5:
      pinkTurquoiseBreatheMode();
      break;
    case 6:
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
      solidWhiteMode();
      break;
    case 14:
      rgbBreatheMode();
      break;
    case 15:
      break;
    // Shifted Second Row
    case 16:
      sunriseMode(2, 5);
      break;
    case 17:
      sunsetMode(0, 5);
      break;
    case 18:
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
  }
}
