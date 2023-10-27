#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

typedef struct
{
    int delta;
    int click;
    int changed;
} Encoder_State_t;

class Encoder
{
public:
    int clkpin;
    int dtpin;
    int swpin;
    unsigned long debounce_ms = 0;

    Encoder_State_t es;
    Encoder(int, int, int);
    void setDial();
    void setSwitch();
    void setDelta(int);
    int getDial();
    int getSwitch();
};
#endif
