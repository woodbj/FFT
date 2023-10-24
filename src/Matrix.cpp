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

int Matrix::pindex(int x, int y)
{
  if (y % 2 == 0)
  {
    return parameters.width * y + (parameters.width - x - 1);
  }
  else
  {
    return parameters.width * y + x;
  }
}

void Matrix::pushSpectLayer()
{
  float fColourIndex;
  int colourIndex;
  float hue;
  float sat = 1;
  float lum = 1;

  static int rbow = 0; // used instead of hue for rainbow modes
  static int counter = 0;

  // scroll old values
  for (int step = parameters.height - 1; step > 0; step--)
  {
    for (int band = 0; band < parameters.width; band++)
    {
      spect[pindex(band, step)] = spect[pindex(band, step - 1)];
    }
  }

  // calculate new values
  for (int band = 0; band < parameters.width; band++)
  {
    fColourIndex = constrain(parameters.bandValues[band], 0, 1);
    hue = 240 - 240 * fColourIndex;
    colourIndex = round(255 * fColourIndex);
    HSLtoRGB(hue, 1, 0.5);

    spect[pindex(band, 0)].setRGB(r, g, b);
  }

  // push to matrix
  for (int led = 0; led < parameters.ledCount; led++)
  {
    leds[led] = spect[led];
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

void Matrix::go()
{
    pushSpectLayer();
    FastLED.show();
}
