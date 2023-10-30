#ifndef UTILITIES_H
#define UTILITIES_H

#include <Arduino.h>

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int fpsActual;

void capFPS(int fps)
{
  static unsigned long startTime = micros();
  int period = 1e6 / fps;
  while (micros() - startTime < period)
  {
  }
  fpsActual = 1e6 / (micros() - startTime);
  startTime = micros();
}

#endif