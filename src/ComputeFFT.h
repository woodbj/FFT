#ifndef COMPUTEFFT_H
#define COMPUTEFFT_H

#include <arduinoFFT.h>
#include <Arduino.h>

#define TWOPI 6.28318530718


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
}ComputeFFT_Parameters_t;


class ComputeFFT{
    private:
    ComputeFFT_Parameters_t parameters;
    arduinoFFT FFT;
    enum Windows currentWindow;

    public:
    ComputeFFT(ComputeFFT_Parameters_t);
    void go(short *);

    void setWindow(enum Windows);
    void applyWindow();
    void prepareInput(short*);
    void getMagnitude();

};



#endif