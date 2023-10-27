#ifndef MATRIX_H
#define MATRIX_H

#include "hardware.h"
#include "letters.h"
#include <Arduino.h>

#define FASTLED_INTERNAL
#include <FastLED.h>


#define LED_TYPE WS2812B

#define MAX_CHAR_LEN 10
typedef enum
{
    SPECTROGRAM,
    COLOUR_BC,
    COLOUR_BCW,
    RAINBOW_BC,
    RAINBOW_BCW,
    LAST_STYLE
}Styles_t;
typedef struct
{
    unsigned char ledPin;
    int xTileCount;
    int yTileCount;
    int xLEDSPerTile;
    int yLEDSPerTile;
    int ledCount;
    float *bandValues;
    int width;
    int height;
} Matrix_Parameters_t;

class Matrix
{
private:
    Matrix_Parameters_t parameters;
    CRGB menuColour = CRGB::White;
    CRGB leds[NUM_LED];  // master LED
    CRGB spect[NUM_LED]; // spectrogram values
    CRGB menu[NUM_LED];
    int mode = SPECTROGRAM;
    unsigned long menuDwell_ms = 2000;
    unsigned long lastMenuDrawTime;

    float h, s, l;
    int r, g, b;

public:
    Matrix(Matrix_Parameters_t);
    float mapF(float, float, float, float, float);
    int pindex(int, int);
    void buildSpectLayer();
    void HSLtoRGB(int, float, float);
    void setMode(int dir);
    void clearMenu();
    void drawString(char*, int, int);
    void mergeLayers();
    void go();
};

#endif
