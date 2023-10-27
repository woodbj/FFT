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

void Encoder::setDelta(int change)
{
    es.delta += change;
}

void Encoder::setSwitch()
{
    es.click = es.click + 1;
}

int Encoder::getDial()
{
    int value = es.delta;
    es.delta = 0;
    return value;
}

int Encoder::getSwitch()
{
    int value = es.click;
    es.click = 0;
    return value;
}
