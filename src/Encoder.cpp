#include "Encoder.h"

Encoder::Encoder(int clockPin, int dataPin, int switchPin)
{
    clkpin = clockPin;
    dtpin = dataPin;
    swpin = switchPin;

    pinMode(clockPin, INPUT);
    pinMode(dataPin, INPUT);
    pinMode(switchPin, INPUT);

    es.delta = 0;
    es.click = 0;
    es.changed = false;
    es.navigating = false;
}

void Encoder::setDial()
{
    bool dt = digitalRead(dtpin);
    bool clk = digitalRead(clkpin);

    if (clk == dt)
    {
        es.delta++;
        es.changed = true;
    }
    else
    {
        es.delta--;
        es.changed = true;
    }
}

void Encoder::setSwitch()
{
    es.click = es.click + 1;
    es.changed = true;
    es.navigating = !es.navigating;
}

Encoder_State_t Encoder::getState()
{
    Encoder_State_t oldState = es;

    es.changed = false;
    es.click = 0;
    es.delta = 0;

    return oldState;
}