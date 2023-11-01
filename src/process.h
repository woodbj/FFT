#ifndef PROCESS_H
#define PROCESS_H

#ifndef Arduino_h
#include <Arduino.h>
#endif

#define C0 16.3515978312874f

enum ScaleMode {
    DB_POWER_DENSITY,
    INT_POWER_DENSITY,
    LAST_SCALE_MODE
};

typedef struct
{
    int sampleRate;
    int sampleCount;
    int bandCount;
    float *bandValues;
    double *vRe;
    int *binsPerBand;
    int *gSampleRate;
} Processor_Parameters_t;

class Processor
{
private:
    Processor_Parameters_t parameters;
    int firstBin;
    int lastBin;
    float gain = 1;
    int loThreshold = 1;
    float hiThreshold = 0.5;
    float volFloor = 0.01;
    float volPeak = 1;
    float volActual = 0.1;
    float nFirst = 30;
    float nLast;
    int notesPerBand = 3;

public:
    Processor(Processor_Parameters_t p);
    void setSampleRate(int);
    void binsToBands();
    float freqToNote(float f) { return 12 * log2f(f / C0); }
    float noteToFreq(float n) { return C0 * powf(2, n / 12.0); }
    float incrementGain(int);
    void setGain(int newgain) { gain = newgain;}
    float incrementVolTarget(int);
    float getVolActual() { return volActual; }
    int incrementLoNote(int);
    int incrementNPB(int);
    int getSampleRate() { return parameters.sampleRate;}
    int buildBins();
    void scale();
    void go();
};

#endif