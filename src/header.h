// v0.1 dual core established and working
// v0.2 Rolling buffer, improvements to processing and display. Works a charm!
// v0.3 Fiddled a bit with processing the bands. It's looking pretty good, saving this version
// v0.4 Implemented a rotary encoder. Can edit things but no navigation menu to speak of.
//      Adjusting to a user-selected static scaling factor improved the visual quality
// v0.5 changed to a struct system for easier modification of user variables
// v0.6 adding in menu lettering function
// v0.7 [BUGGY] added a dynamic menu to better account for style-specific settings. loses track of the style settings
// v0.8 added palette selections. Fixed the v0.7 bug


// General inclusions
#include <Arduino.h>
#include <math.h>
#include "letters.h"
#include "microphone.h"

// LED Matrix
#define FASTLED_INTERNAL
#include <FastLED.h>
#define LED_PIN 11
#define MAT_W 16
#define MAT_H 16
#define NUM_LED (MAT_W * MAT_H)
float fps = 25;
float per = 1.0f / fps * 1e6;
float h, s, l;
int r, g, b;
CRGB leds[NUM_LED];

CRGB spect[NUM_LED];
int title[NUM_LED];
#define TWOPI 6.28318530718
#define ONEPI 3.14159265358

// FFT
#include <arduinoFFT.h>
#define SAMPLES 1024
#define SAMPLE_FREQ 30000.0f  //25140.0f
float uvSAMPLERATE = SAMPLE_FREQ;
int firstBin;                 // value set by void buildBins()
int binsPerBand[MAT_W] = { 2, 2, 2, 2, 2, 2, 2, 2, 3, 4, 5, 5, 7, 7, 9, 11 };
double vRe[SAMPLES];
double vIm[SAMPLES];
arduinoFFT FFT = arduinoFFT(vRe, vIm, SAMPLES, SAMPLE_FREQ);
unsigned int sampling_period_us = round(1e6 / SAMPLE_FREQ);

enum Windows {
  RECTANGULAR,
  TRIANGULAR,
  SINE,
  HAMMING,
  HANNING,
  WINDOW_COUNT
};

float wj[WINDOW_COUNT][SAMPLES];

// // Microphone
// #include <driver/adc.h>
// #define MIC_PIN ADC1_CHANNEL_1  //GPIO2
// const int bufferLen = 2 * SAMPLES;
// int bufferIndex = 0;
// double buffer[bufferLen];

// Bands
float bandValues[MAT_W];
const int avgSamples = 100;
int avgCounter = 0;
float integral[avgSamples];

// Encoder
#define CLK 13  // blue wire
#define SW 14   // yellow wire
#define DT 15   // white wire
bool swPress = false;
bool enIncrement = false;
bool enDecrement = false;
bool fastTurn = false;
int fastMult = 30;
unsigned int lastClick = millis();
unsigned int enCounter = 0;
int debounceLastTime = millis();
int debounce_ms = 5;

// user variables
#include "palettes.h"
CRGB menuColour = CRGB::White;

enum Menu {
  STYLE,
  STYLE_SETTING,
  FLOW,
  FHIGH,
  // SAMPLERATE,
  YSCALE,
  // WINDOW,
  GAIN,
  VOLUME,
  MENU_COUNT
};

enum Y_Scales {
  DB_POWER_DENSITY,
  INT_POWER_DENSITY,
  LAST_YSCALE
};

enum Styles {
  SPECTROGRAM,
  COLOUR_BC,
  RAINBOW_BC,
  COLOUR_BCW,
  RAINBOW_BCW,
  LAST_STYLE
};

float uvSTYLE = SPECTROGRAM;
float uvFLOW = 32;
float uvFHIGH = 80;

float uvYSCALE = INT_POWER_DENSITY;
float uvWINDOW = HANNING;
float uvGAIN = 1;
float uvVOLUME = 7;

float uvSTYLE_SETTING_SPECTROGRAM = 0;
float uvSTYLE_SETTING_COLOUR_BC = 0;
float uvSTYLE_SETTING_RAINBOW_BC = 0;


#define TITLE_LEN 20
typedef struct {
  float *ptr;
  float val;
  float min;
  float max;
  float delta;
  float fastDelta;
  bool fastEnable = false;
  bool rollover = false;
  bool showValueBar = true;
  bool reverseValueBar = false;
  char title[TITLE_LEN];
  bool changed = true;
} UserVar_t;

UserVar_t uv[MENU_COUNT];
UserVar_t styleSettings[LAST_STYLE];


float uvSampleAvg = 0;

Cursor_t cursor;

int menuSelectTime = -5000;  // used to only show the menu bar for a set time
int menuNum = 0;             // current menu being navigated (refer enum Menu)
bool selectMenu = true;      // track whether it's a menu or variable being manipulated
bool menuGo = false;         // for ISR to tell the loop if a menu action needs to be taken

// used to manage the value bar
float currentMin;
float currentMax;
float currentValue;
int currentIndex;

// RTOS
#define STACK_SIZE 2*4096
TaskHandle_t sampler;
TaskHandle_t computer;


#define I2S_WS 18
#define I2S_SCK 33
#define I2S_SD 34



Microphone mic;

int16_t samples[SAMPLES];


#include "utilities.h"
#include "1_compute.h"
#include "2_process.h"
#include "3_display.h"
#include "4_encoder.h"