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
#include "process.h"
#include "ComputeFFT.h"

#define SAMPLES 1024
#define SAMPLE_FREQ 31250 // 25140.0f

#define I2S_WS 18
#define I2S_SCK 33
#define I2S_SD 34

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

// Microphone
sampletype_t samples[SAMPLES];

Mic_Settings_t mic_settings = {
  .ws = I2S_WS,
  .sck = I2S_SCK,
  .sd = I2S_SD,
  .sample_rate = SAMPLE_FREQ,
  .sample_count = SAMPLES};

Microphone mic = Microphone(mic_settings);

// FFT
double vRe[SAMPLES];
double vIm[SAMPLES];
double win[SAMPLES];

ComputeFFT_Parameters_t fftparam = {
  .sampleCount = SAMPLES,
  .sampleRate = SAMPLE_FREQ,
  .real = vRe,
  .imag = vIm,
  .windowArray = win,
  .window = HAMMING};

ComputeFFT fft = ComputeFFT(fftparam);


// Bands
float bandValues[MAT_W];
int binsPerBand[MAT_W];
Processor_Parameters_t procparam = {
    .sampleRate = SAMPLE_FREQ,
    .sampleCount = SAMPLES,
    .bandCount = MAT_W,
    .bandValues = bandValues,
    .vRe = vRe,
    .binsPerBand = binsPerBand};

Processor processor = Processor(procparam);

// user variables
#include "palettes.h"
CRGB menuColour = CRGB::White;



#define TITLE_LEN 20


// RTOS
#define STACK_SIZE 2 * 4096
TaskHandle_t processtask;
TaskHandle_t computer;
QueueHandle_t queue;

typedef enum
{
  READY_TO_PROCESS
} Queue_Message_t;


#include "utilities.h"
#include "3_display.h"