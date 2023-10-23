int pindex(int x, int y)
{
  if (y % 2 == 0)
  {
    return MAT_W * y + (MAT_W - x - 1);
  }
  else
  {
    return MAT_W * y + x;
  }
}

void hslToRgb(int h, float s, float l)
{ // Cheers, ChatGPT!
  float c = (1 - abs(2 * l - 1)) * s;
  float x = c * (1 - abs(fmod(h / 60.0, 2) - 1));
  float m = l - c / 2;
  float rTemp, gTemp, bTemp;

  if (h >= 0 && h < 60)
  {
    rTemp = c;
    gTemp = x;
    bTemp = 0;
  }
  else if (h >= 60 && h < 120)
  {
    rTemp = x;
    gTemp = c;
    bTemp = 0;
  }
  else if (h >= 120 && h < 180)
  {
    rTemp = 0;
    gTemp = c;
    bTemp = x;
  }
  else if (h >= 180 && h < 240)
  {
    rTemp = 0;
    gTemp = x;
    bTemp = c;
  }
  else if (h >= 240 && h < 300)
  {
    rTemp = x;
    gTemp = 0;
    bTemp = c;
  }
  else
  {
    rTemp = c;
    gTemp = 0;
    bTemp = x;
  }

  r = (int)((rTemp + m) * 255);
  g = (int)((gTemp + m) * 255);
  b = (int)((bTemp + m) * 255);
}

// void pushMenuLayer() {
//   char let;
//   Cursor_t c;
//   c.x = 0;   // uv[menuNum].cur.x;
//   c.y = 10;  // uv[menuNum].cur.y;

//   for (int i = 0; i < TITLE_LEN; i++) {
//     // read the letter
//     let = uv[menuNum].title[i];  // get the letter from the char array
//     if (let == 0) break;         // if null exit the loop

//     // declarations
//     int pixel;
//     int letter = letters[let - 'A'];
//     int width = 0;
//     int bit = 0;
//     bool inMatrix;

//     // print the letter
//     for (int y = 0; y < LETTER_HEIGHT; y++) {
//       for (int x = 0; x < LETTER_WIDTH; x++) {
//         pixel = (letter >> bit) & 1;                        // bitshifts to the current bit under examination then masks it with a 0b1 to clear all but the last bit
//         inMatrix = (x + c.x < MAT_W) && (y + c.y < MAT_H);  // check to see the letter doesn't overflow the matrix boundaries
//         if (pixel && inMatrix) leds[pindex(x + c.x, y + c.y)] = menuColour;
//         if (pixel && x > width) width = x;
//         bit++;
//       }
//     }
//     c.x += width + 2;
//   }
// }

void pushSpectLayer()
{

  float fColourIndex;
  int colourIndex;
  float hue;
  float sat = 1;
  float lum = 1;

  static int rbow = 0; // used instead of hue for rainbow modes
  static int counter = 0;

  // scroll old values
  for (int step = MAT_H - 1; step > 0; step--)
  {
    for (int band = 0; band < MAT_W; band++)
    {
      spect[pindex(band, step)] = spect[pindex(band, step - 1)];
    }
  }

  // calculate new values
  for (int band = 0; band < MAT_W; band++)
  {
    fColourIndex = constrain(bandValues[band], 0, 1);

    colourIndex = round(255 * fColourIndex);
    spect[pindex(band, 0)] = ColorFromPalette(palletes[0], colourIndex, 255, LINEARBLEND_NOWRAP);
  }

  // push to matrix
  for (int led = 0; led < NUM_LED; led++)
  {
    leds[led] = spect[led];
  }
}

void displaySamples()
{
  pushSpectLayer();
  FastLED.show();
}