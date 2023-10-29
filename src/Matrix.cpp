#include "Matrix.h"

Matrix::Matrix(Matrix_Parameters_t _parameters)
{
  parameters = _parameters;
  parameters.width = parameters.xTileCount * parameters.xLEDSPerTile;
  parameters.height = parameters.xTileCount * parameters.xLEDSPerTile;
  parameters.ledCount = parameters.width * parameters.height;

  FastLED.addLeds<LED_TYPE, LED_PIN, GRB>(leds, NUM_LED);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
  FastLED.setBrightness(100);
}

float Matrix::mapF(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int Matrix::pindex(int x, int y)
{

  /*
    Top left is (0,0)
    x increases left to right
    y increases top to bottom
  */
  if (y % 2 == 0)
  {
    return parameters.width * y + (parameters.width - x - 1);
  }
  else
  {
    return parameters.width * y + x;
  }
}

void Matrix::buildSpectLayer()
{

  float fColourIndex;
  float hue;
  float sat = 1;
  float lum = 1;

  static int rbow = 0; // used instead of hue for rainbow modes
  static int counter = 0;

  // scroll old values
  for (int step = parameters.width - 1; step > 0; step--)
  {
    for (int band = 0; band < parameters.width; band++)
    {
      spect[pindex(band, step)] = spect[pindex(band, step - 1)];
    }
  }

  // calculate new values
  for (int band = 0; band < parameters.width; band++)
  {
    // build hsl and rgb values based on the mode
    fColourIndex = parameters.bandValues[band];
    switch (mode)
    {
    case SPECTROGRAM:
      hue = mapF(fColourIndex, 0, 1, 240, 0);
      lum = 0.5;
      break;

    case COLOUR_BC:
      hue = colourMode;
      lum = 0.5 * fColourIndex;
      break;

    case COLOUR_BCW:
      hue = colourMode;
      lum = 1 * fColourIndex;
      break;

    case RAINBOW_BC:
      hue = rbow;
      lum = 0.5 * fColourIndex;
      break;

    case RAINBOW_BCW:
      hue = rbow;
      lum = 1 * fColourIndex;
      break;
    }

    HSLtoRGB(hue, sat, lum);
    spect[pindex(band, 0)] = CRGB(r, g, b);

    // prepare the iterator for the next rainbow hue
    counter++;
    if (counter % (rainbowRate + 1) == 0)
    {
      rbow++;
      if (rbow >= 360)
        rbow = 0;
    }
  }
}

void Matrix::HSLtoRGB(int h, float s, float l)
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

void Matrix::scrollMode(int dir)
{
  int currentMode = mode;
  currentMode += dir;
  if (currentMode < 0)
    currentMode = 0;
  else if (currentMode >= LAST_STYLE)
    currentMode = LAST_STYLE - 1;
  mode = currentMode;
}

int Matrix::getCurrentMode()
{
  return mode;
}

int Matrix::incrementColour(int dir)
{
  colourMode += dir;
  if (colourMode < 0)
    colourMode += 360;
  colourMode %= 360;
  return colourMode;
}

int Matrix::incrementRainbowRate(int dir)
{
  int min = 0;
  int max = 20;
  rainbowRate += dir;
  if (rainbowRate < min)
    rainbowRate = min;
  if (rainbowRate > max)
    rainbowRate = max;
  return rainbowRate;
}

void Matrix::getModeString(char *str)
{
  switch (mode)
  {
  case SPECTROGRAM:
    strcpy(str, "SPEC");
    break;
  case COLOUR_BC:
    strcpy(str, "col1");
    break;
  case COLOUR_BCW:
    strcpy(str, "col2");
    break;
  case RAINBOW_BC:
    strcpy(str, "rb1");
    break;
  case RAINBOW_BCW:
    strcpy(str, "rb2");
    break;
  default:
    break;
  }
}

void Matrix::setMenuColour(CRGB newCol)
{
  menuColour = newCol;
}

void Matrix::clearMenu()
{
  for (int i = 0; i < NUM_LED; i++)
  {
    menu[i] = CRGB::Black;
  }
}

void Matrix::drawString(char *string, int xpos, int ypos)
{
  int letterVal;
  int bitIndex;
  int pixel;
  int letterWidth;

  int sx = xpos;
  int sy = ypos;

  int absx;
  int absy;

  CRGB pixCol;

  for (int i = 0; i < MAX_CHAR_LEN; i++)
  {
    if (string[i] == 0)
    { // terminate the loop if at the null character at the end of the string
      break;
    }
    bitIndex = 0;
    letterWidth = 0;
    letterVal = asciiFont[string[i]];

    if (letterVal == 0)
    { // skip the character if there is not font identified for it
      continue;
    }

    for (int ly = 0; ly < LETTER_HEIGHT; ly++)
    {
      for (int lx = 0; lx < LETTER_WIDTH; lx++)
      {
        absx = sx + lx;
        absy = sy + ly;
        if (absx >= parameters.width || absy >= parameters.height)
        { // skip this pixel if outside the matrix boundaries
          bitIndex++;
          continue;
        }

        // get the pixel value
        pixel = (letterVal >> bitIndex) & 0b1;

        if (pixel)
        { // if pixel index value is a 1, draw it
          pixCol = menuColour;

          if (lx > letterWidth)
          { // track the width of the letter
            letterWidth = lx;
          }
        }
        else
        { // if not, draw in a black pixel
          pixCol = CRGB::Black;
        }

        menu[pindex(absx, absy)] = pixCol;

        bitIndex++;
      }
    }
    sx += letterWidth + 2;
  }

  // return sx;

  lastMenuDrawTime = millis();
}

void Matrix::drawDecimal(float input, int xpos, int ypos, int sigfig)
{
  // convert float to string
  char str[MAX_CHAR_LEN];

  int sign = abs(input) / input;
  input = abs(input);
  int order = log10(input) + 1;
  input /= pow(10, order - 1);

  int index = 0;

  int iterations = sigfig;
  if (order > sigfig)
  {
    iterations = order;
  }

  for (int i = 0; i < MAX_CHAR_LEN; i++)
  { // zero out the string
    str[i] = 0;
  }

  if (sign < 0)
  { // add a negative sign if neccessary
    str[index] = '-';
    index++;
  }

  // if (order < 1)
  // { // add leading zero if needed
  //   str[index] = '0';
  //   index++;
  // }

  if (order < 1 && iterations > 1)
  { // add decimal point after leading zero if needed
    str[index] = '.';
    index++;
  }

  double num = input;
  int digit;
  for (int i = 0; i < iterations; i++)
  {
    digit = num; // conversion from double to int will cut decimals
    str[index] = digit + '0';

    if (order == 1 && i < iterations - 1)
    { // add decimal. Ignore if on the last iteration
      index++;
      str[index] = '.';
    }

    order--;
    index++;
    num = (num - digit) * 10;
  }

  drawString(str, xpos, ypos);
}

void Matrix::mergeLayers()
{
  for (int i = 0; i < NUM_LED; i++)
  {
    leds[i] = spect[i];
    if (millis() - lastMenuDrawTime < menuDwell_ms)
    {
      if (menu[i] != CRGB::Black)
      {
        leds[i] = menu[i];
      }
      else
      {
        float dim = 0.25;
        int r = dim * leds[i].red;
        int g = dim * leds[i].green;
        int b = dim * leds[i].blue;
        leds[i].setRGB(r, g, b);
      }
    }
  }
}

void Matrix::go()
{
  buildSpectLayer();
  mergeLayers();
  FastLED.show();
}
