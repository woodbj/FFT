#ifndef COMPUTEFFT_H
#define COMPUTEFFT_H

#include <arduinoFFT.h>
#include <Arduino.h>

#define TWOPI 6.28318530718
#define C0 16.3515978312874f


enum Windows {
  RECTANGULAR,
  TRIANGULAR,
  SINE,
  HAMMING,
  HANNING,
  WINDOW_COUNT
};

typedef struct{
    int sampleCount;
    int sampleRate;
    double* real;
    double* imag;
    double* windowArray;
    enum Windows window;
    bool *resetSR;
    int *newSR;
}ComputeFFT_Parameters_t;


class ComputeFFT{
    private:
    ComputeFFT_Parameters_t parameters;
    arduinoFFT FFT;
    enum Windows currentWindow;

    public:
    ComputeFFT(ComputeFFT_Parameters_t);

    void setWindow(enum Windows);
    void applyWindow();
    void prepareInput(short*);
    void getMagnitude();
    void setSampleRate(int);
    int getSampleRate() { return parameters.sampleRate;}
    void go(short *);

};



#endif