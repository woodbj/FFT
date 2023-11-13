// General inclusions
#ifndef MY_HEADER_H
#define MY_HEADER_H

#include <Arduino.h>
#include <math.h>
#include "hardware.h"
#include "utilities.h"

// initial conditions
#define SAMPLES 1024
#define SAMPLE_FREQ 10000 // 25140.0f
#define FPS 40

// variables used across 
int fpsRequested = FPS;
int masterSamplingRate = SAMPLE_FREQ;
bool masterSamplingReset = true;



// Microphone
#include "microphone.h"
sampletype_t samples[SAMPLES];


Mic_Settings_t mic_settings = {
    .ws = I2S_WS,
    .sck = I2S_SCK,
    .sd = I2S_SD,
    .sample_rate = SAMPLE_FREQ,
    .sample_count = SAMPLES,
    .gSampleRate = &masterSamplingRate,
    .resetSR = &masterSamplingReset,
    .newSR = &masterSamplingRate};

Microphone mic = Microphone(mic_settings);

// FFT
#include "ComputeFFT.h"
double vRe[SAMPLES];
double vIm[SAMPLES];
double win[SAMPLES];

ComputeFFT_Parameters_t fftparam = {
    .sampleCount = SAMPLES,
    .sampleRate = SAMPLE_FREQ,
    .real = vRe,
    .imag = vIm,
    .windowArray = win,
    .window = HAMMING,
    .resetSR = &masterSamplingReset,
    .newSR = &masterSamplingRate};

ComputeFFT fft = ComputeFFT(fftparam);

// Processor

#include "process.h"
float bandValues[BAND_COUNT];
int binsPerBand[BAND_COUNT];

Processor_Parameters_t procparam = {
    .sampleRate = SAMPLE_FREQ,
    .sampleCount = SAMPLES,
    .bandCount = BAND_COUNT,
    .bandValues = bandValues,
    .vRe = vRe,
    .binsPerBand = binsPerBand,
    .resetSR = &masterSamplingReset,
    .newSR = &masterSamplingRate};

Processor processor = Processor(procparam);


// Matrix
#include "Matrix.h"
#define FASTLED_INTERNAL
#include <FastLED.h>

Matrix_Parameters_t matparam = {
    .ledPin = LED_PIN,
    .xTileCount = X_TILES,
    .yTileCount = Y_TILES,
    .xLEDSPerTile = X_LED_PER_TILE,
    .yLEDSPerTile = Y_LED_PER_TILE,
    .ledCount = NUM_LED,
    .bandValues = bandValues};

Matrix matrix = Matrix(matparam);

// Encoder
#include "Encoder.h"
Encoder_State_t gEncoderState;
Encoder enc = Encoder(ENC_CLK, ENC_DT, ENC_SW);

// Menu
#include "MenuManager.h"
MenuManager menu;
#include "menuFunctions.h"

// RTOS
#define STACK_SIZE 2 * 4096
TaskHandle_t processtask;
TaskHandle_t computer;
QueueHandle_t queue;

typedef enum
{
    READY_TO_PROCESS
} Queue_Message_t;

#endif