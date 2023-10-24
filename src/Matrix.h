#ifndef MATRIX_H
#define MATRIX_H

#include "hardware.h"

#define FASTLED_INTERNAL
#include <FastLED.h>

#define LED_TYPE WS2812B

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

// DEFINE_GRADIENT_PALETTE(spectrogram_gp){
//   0, 0, 0, 255,    
//   64, 0, 255, 255,   
//   128, 0, 255, 0,   
//   192, 255, 255, 0,  
//   255, 255, 0, 0
// };  

class Matrix
{
private:
    Matrix_Parameters_t parameters;
    CRGB menuColour = CRGB::White;
    CRGB leds[NUM_LED];     // master LED
    CRGB spect[NUM_LED];    // spectrogram values

    float h, s, l;
    int r, g, b;

public:
    Matrix(Matrix_Parameters_t);
    int pindex(int, int);
    void go();
    void pushSpectLayer();
    void HSLtoRGB(int, float, float);
};

#endif

