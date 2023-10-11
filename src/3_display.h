int pindex(int x, int y) {
  if (y % 2 == 0) {
    return MAT_W * y + (MAT_W - x - 1);
  } else {
    return MAT_W * y + x;
  }
}

void hslToRgb(int h, float s, float l) {  //Cheers, ChatGPT!
  float c = (1 - abs(2 * l - 1)) * s;
  float x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
  float m = l - c / 2;
  float rTemp, gTemp, bTemp;

  if (h >= 0 && h < 60) {
    rTemp = c;
    gTemp = x;
    bTemp = 0;
  } else if (h >= 60 && h < 120) {
    rTemp = x;
    gTemp = c;
    bTemp = 0;
  } else if (h >= 120 && h < 180) {
    rTemp = 0;
    gTemp = c;
    bTemp = x;
  } else if (h >= 180 && h < 240) {
    rTemp = 0;
    gTemp = x;
    bTemp = c;
  } else if (h >= 240 && h < 300) {
    rTemp = x;
    gTemp = 0;
    bTemp = c;
  } else {
    rTemp = c;
    gTemp = 0;
    bTemp = x;
  }

  r = (int)((rTemp + m) * 255);
  g = (int)((gTemp + m) * 255);
  b = (int)((bTemp + m) * 255);
}

void pushMenuLayer() {
  char let;
  Cursor_t c;
  c.x = 0;   // uv[menuNum].cur.x;
  c.y = 10;  // uv[menuNum].cur.y;

  for (int i = 0; i < TITLE_LEN; i++) {
    // read the letter
    let = uv[menuNum].title[i];  // get the letter from the char array
    if (let == 0) break;         // if null exit the loop

    // declarations
    int pixel;
    int letter = letters[let - 'A'];
    int width = 0;
    int bit = 0;
    bool inMatrix;

    // print the letter
    for (int y = 0; y < LETTER_HEIGHT; y++) {
      for (int x = 0; x < LETTER_WIDTH; x++) {
        pixel = (letter >> bit) & 1;                        // bitshifts to the current bit under examination then masks it with a 0b1 to clear all but the last bit
        inMatrix = (x + c.x < MAT_W) && (y + c.y < MAT_H);  // check to see the letter doesn't overflow the matrix boundaries
        if (pixel && inMatrix) leds[pindex(x + c.x, y + c.y)] = menuColour;
        if (pixel && x > width) width = x;
        bit++;
      }
    }
    c.x += width + 2;
  }
}

void pushSpectLayer() {
  int mode = uv[STYLE].val;
  float setting = styleSettings[mode].val;

  float fColourIndex;
  int colourIndex;
  float hue = setting;
  float sat = 1;
  float lum = 1;

  static int rbow = 0;  // used instead of hue for rainbow modes
  static int counter = 0;
  int palette = (int)styleSettings[SPECTROGRAM].val;

  // scroll old values
  for (int step = MAT_H - 1; step > 0; step--) {
    for (int band = 0; band < MAT_W; band++) {
      spect[pindex(band, step)] = spect[pindex(band, step - 1)];
    }
  }

  // calculate new values
  for (int band = 0; band < MAT_W; band++) {
    fColourIndex = constrain(bandValues[band], 0, 1);

    // build hsl and rgb values based on the mode
    switch (mode) {
      case SPECTROGRAM:
        colourIndex = round(255 * fColourIndex);
        spect[pindex(band, 0)] = ColorFromPalette(palletes[palette], colourIndex, 255, LINEARBLEND_NOWRAP);
        break;

      case COLOUR_BC:
        lum = 0.5 * fColourIndex;
        hslToRgb(hue, sat, lum);
        spect[pindex(band, 0)] = CRGB(r, g, b);
        break;

      case COLOUR_BCW:
        lum = 1 * fColourIndex;
        hslToRgb(hue, sat, lum);
        spect[pindex(band, 0)] = CRGB(r, g, b);
        break;

      case RAINBOW_BC:
        hue = rbow;
        lum = 0.5 * fColourIndex;
        hslToRgb(hue, sat, lum);
        spect[pindex(band, 0)] = CRGB(r, g, b);
        break;

      case RAINBOW_BCW:
        hue = rbow;
        lum = 1 * fColourIndex;
        hslToRgb(hue, sat, lum);
        spect[pindex(band, 0)] = CRGB(r, g, b);
        break;
    }
    
    // prepare the iterator for the next rainbow hue
    int rate = constrain(setting, 0, 50);
    counter++;
    if (counter % (rate + 1) == 0) {
      rbow++;
      if (rbow >= 360) rbow = 0;
    }
  }

  // push to matrix
  for (int led = 0; led < NUM_LED; led++) {
    leds[led] = spect[led];
  }
}

void pushValueLayer() {
  if (uv[menuNum].showValueBar) {
    float min = 1.0f * currentMin;
    float max = 1.0f * currentMax;
    float val = 1.0f * currentValue;
    bool cond;

    val = mapFloat(val, min, max, 0, MAT_W) - 1;
    val = round(val);

    for (int i = 0; i < MAT_W; i++) {
      if (uv[menuNum].reverseValueBar) cond = i >= val;
      else cond = i <= val;
      if (cond) {
        leds[pindex(i, MAT_H - 1)] = menuColour;
      } else leds[pindex(i, MAT_H - 1)] = CRGB::Black;
    }
  }
}

void displaySamples() {
  pushSpectLayer();

  bool timer = millis() - menuSelectTime < 3000;
  if (timer) {
    if (selectMenu) {
      pushMenuLayer();
    } else {
      pushValueLayer();
    }
  }
  FastLED.show();
}