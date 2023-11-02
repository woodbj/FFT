/*
This header includes everything needed to install and run an i2s microphone
*/
#ifndef MICROPHONE_H
#define MICROPHONE_H
#include <driver/i2s.h>

// dependencies
#ifndef Arduino_h
#include <Arduino.h>
#endif

#define SAMPLES_PER_DMA_BUFFER 64
#define BITS_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT
#define BYTES_PER_SAMPLE (BITS_PER_SAMPLE / 8)
#define BYTES_PER_DMA_BUFFER (SAMPLES_PER_DMA_BUFFER * BYTES_PER_SAMPLE)
#define I2S_PORT I2S_NUM_0

typedef short sampletype_t;

typedef struct
{
    int ws;
    int sck;
    int sd;
    unsigned int sample_rate;
    int sample_count;
    int *gSampleRate;
    bool* resetSR;
    int *newSR;

} Mic_Settings_t;

class Microphone
{
private:
    Mic_Settings_t settings;
    int dma_buffer_count;

public:
    Microphone(Mic_Settings_t);
    void micInstall();
    void getBuffer(sampletype_t *);
    float getSampleRate();
    void setSampleRate(uint32_t);
};

#endif