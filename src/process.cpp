#include <process.h>
#include <Arduino.h>



void Processor::yAxis()
{
  float res = parameters.sampleRate / (1.0f * parameters.sampleCount);
  float binf;

  for (int i = 1; i <= parameters.sampleCount / 2; i++)
  {

      binf = i * res;
      if (parameters.vRe[i] < 0)
        parameters.vRe[i] = 0;
      parameters.vRe[i] = 1.25 * parameters.vRe[i] / sqrt(binf);
  }
}

void Processor::scale()
{
  // Scale the bandValues
  float val;
  int counter = 0;
  static float vol = 1;
  float volDelta = 0.01;
  int loThreshold = 1;
  int hiThreshold = 6;

  for (int band = 0; band < parameters.bandCount; band++)
  {
    val = parameters.bandValues[band];
    val = powf(val, gain); // add non-linearity to the response
    val *= vol;              // scale
    if (val > 1.0)
      val = 1.0; // set peak value at 1
    parameters.bandValues[band] = val;
    if (val == 1)
      counter++;
  }

  if (counter > hiThreshold)
  {
    vol -= (counter - hiThreshold) * volDelta;
  }
  else if (counter < loThreshold)
  {
    vol += (loThreshold - counter) * volDelta;
  }
  vol = constrain(vol, 0.2, 7);
}

Processor::Processor(Processor_Parameters_t p)
{
    parameters = p;
    buildBins();
}

void Processor::setSampleRate(int newSampleRate)
{
  parameters.sampleRate = newSampleRate;
  buildBins();
}

void Processor::binsToBands()
{

  // zero out the bands for filling
  for (int i = 0; i < parameters.bandCount; i++)
  {
    parameters.bandValues[i] = 0;
  }

  // fill bands and integral
  int startBin = firstBin;
  for (int band = 0; band < parameters.bandCount; band++)
  {

    // add each applicable bin to respective bands
    float val;
    int endBin = startBin + parameters.binsPerBand[band] - 1;
    for (int bin = startBin; bin <= endBin; bin++)
    {
      val = parameters.vRe[bin];
      // val = log10(val + 1);
      parameters.bandValues[band] += val;
    }
    startBin = endBin + 1;
  }
}

double Processor::incrementGain(int dir)
{
  double min = 0.5;
  double max = 1.5;
  double step = 0.01 * dir;

  gain += step;
  if(gain < min) gain = min;
  if(gain > max) gain = max;

  return gain;
}

void Processor::buildBins()
{ // can count beyond samples/2 so needs some fixing
  float C_0 = 16.3515978312874;
  float sampleRate = parameters.sampleRate;
  float res = sampleRate / (1.0f * parameters.sampleCount);
  int minBins = 1;
  float nFirst = 32;
  float nLast = 80;

  // find the starting conditions
  float fnFirst = C_0 * powf(2, nFirst / 12.0f); // determine the frequency of the first requested note
  int bnFirst = fnFirst / res;                   // determine the bin it fits in
  if (bnFirst < 1)
    bnFirst = 1;                     // if that bin is lower than bin 1, set to bin 1
  float fFirst = bnFirst * res;      // determine the frequency of that bin
  nFirst = 12 * log2f(fFirst / C_0); // convert it to a note

  // find the ending conditions
  float fnLast = C_0 * powf(2, nLast / 12.0f);
  float fsLast = sampleRate / 2;
  float fLast = fnLast;
  if (fsLast < fLast)
    fLast = fsLast;
  int bLast = fLast / res + 1;
  fLast = bLast * res;
  nLast = 12 * log2f(fLast / C_0);
  // find the notes per band
  float nPerBand = (nLast - nFirst) / parameters.bandCount;
  float nStart = nFirst;
  float fStart = fFirst;
  int bStart = bnFirst;
  int bEnd;
  float fEnd;
  float nEnd;
  int bCount;
  firstBin = bStart;
  for (int band = 0; band < parameters.bandCount; band++)
  {
    // first bStart before loop, subsequent at the end of the loop
    fStart = res * bStart;
    // nStart = freqToNote(fStart);
    nEnd = nStart + nPerBand;
    fEnd = noteToFreq(nEnd);
    bEnd = fEnd / res;
    bCount = bEnd - bStart + 1;
    if (bCount < minBins)
      bCount = minBins;
    bEnd = bStart + bCount - 1;
    if (band == parameters.bandCount - 1)
    {
      while (bEnd > parameters.sampleCount / 2)
      {
        bCount--;
        bEnd--;
      }
    }
    parameters.binsPerBand[band] = bCount;
    bStart = bEnd + 1;
    nStart = nEnd;
  }
}

void Processor::go()
{
  yAxis();
  binsToBands();
  scale();
}