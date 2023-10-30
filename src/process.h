#ifndef PROCESS_H
#define PROCESS_H

#ifndef Arduino_h
#include <Arduino.h>
#endif

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
    float volFloor = 0.2;
    float volPeak = 7;


public:
    Processor(Processor_Parameters_t p);
    void setSampleRate(int);
    void binsToBands();
    float freqToNote(float f) { return 12 * log2f(f / 16.3515978312874); }
    float noteToFreq(float n) { return 16.3515978312874 * powf(2, n / 12.0); }
    double incrementGain(int);
    int incrementLoThreshold(int);
    int incrementHiThreshold(int);
    float incrementVolPeak(int);
    void buildBins();
    void scale();
    void yAxis();
    void go();
};

#endif