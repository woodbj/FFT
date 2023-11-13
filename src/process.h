#ifndef PROCESS_H
#define PROCESS_H

#ifndef Arduino_h
#include <Arduino.h>
#endif

#define C0 16.3515978312874f



enum VolumeMode {
    STATIC,
    AUTO,
    LAST_VOL_MODE
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
    bool*resetSR;
    int*newSR;
} Processor_Parameters_t;

typedef struct
{
    float currentMin;
    float currentMax;
    float currentAvg;
    float allTimeMin = __FLT_MAX__;
    float allTimeMax = 0;
} FFT_Stats_t;

class Processor
{
private:
    Processor_Parameters_t parameters;
    FFT_Stats_t stats;
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
    int volMode = STATIC;
    bool ignoreSRUpdate = false;

    float dbfsCeiling = -50;
    float dbfsBand = 20;
    float dbfsFloor;

    float floorAdjust[16] = {
        0.191418455882413, 	
        1.22203022058825, 	
        2.18962669117656, 	
        -1.36495919117645, 	
        2.09865022058825, 	
        0.851461985294165, 	
        -1.79095448529411, 	
        -8.556300367647, 	
        -5.12030742647056, 	
        -5.02930860294113, 	
        0.856685514705944, 	
        1.07220316176472, 	
        1.85929727941175, 	
        2.56212080882358, 	
        3.86794433823532, 	
        5.09039139705885};


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
    float incrementDBFSFloor(int);
    float incrementDBFSCeiling(int);
    float incrementDBFSBand(int);
    int incrementVolMode(int);
    float incrementVolume(int);
    int incrementLoNote(int);
    int incrementNPB(int);
    int getSampleRate() { return parameters.sampleRate;}
    void resetStats();
    int buildBins();
    void rawToDPFS();
    void scale();
    void go();
};

#endif