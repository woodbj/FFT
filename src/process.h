#ifndef PROCESS_H
#define PROCESS_H

#ifndef Arduino_h
#include <Arduino.h>
#endif

#define C0 16.3515978312874f



typedef struct
{
    int sampleRate;
    int sampleCount;
    int bandCount;
    float *bandValues;
    double *vRe;
    int *binsPerBand;
} Processor_Parameters_t;

class Processor
{
private:
    Processor_Parameters_t parameters;
    int firstBin;
    float gain = 1;
    int loThreshold = 1;
    int hiThreshold = 6;
    float volFloor = 0.01;
    float volPeak = 7;
    float volActual = 3;
    float nFirst = 33;
    float nLast;
    int notesPerBand = 2;
    float osrMin = 1;

public:
    Processor(Processor_Parameters_t p);
    void setSampleRate(int);
    void binsToBands();
    float freqToNote(float f) { return 12 * log2f(f / C0); }
    float noteToFreq(float n) { return C0 * powf(2, n / 12.0); }
    double incrementGain(int);
    int incrementLoThreshold(int);
    int incrementHiThreshold(int);
    float incrementVolPeak(int);
    float getVolActual() { return volActual; }
    int incrementLoNote(int);
    int incrementHiNote(int);
    int incrementNPB(int);
    float getSampleRate() { return parameters.sampleRate;}
    int buildBins2();
    void buildBins();
    void scale();
    void yAxis();
    void go();
};

#endif