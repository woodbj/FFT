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
  float volDelta = 0.01;

  for (int band = 0; band < parameters.bandCount; band++)
  {
    val = parameters.bandValues[band];
    val = powf(val, gain); // add non-linearity to the response
    val *= volActual;            // scale
    if (val > 1.0)
      val = 1.0; // set peak value at 1
    parameters.bandValues[band] = val;
    if (val == 1)
      counter++;
  }

  if (counter > hiThreshold)
  {
    volActual -= (counter - hiThreshold) * volDelta;
  }
  else if (counter < loThreshold)
  {
    volActual += (loThreshold - counter) * volDelta;
  }
  volActual = constrain(volActual, volFloor, volPeak);
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
  if (gain < min)
    gain = min;
  if (gain > max)
    gain = max;

  return gain;
}

int Processor::incrementLoThreshold(int dir)
{
  loThreshold += dir;
  loThreshold = constrain(loThreshold, 0, hiThreshold);
  return loThreshold;
}

int Processor::incrementHiThreshold(int dir)
{
  hiThreshold += dir;
  hiThreshold = constrain(hiThreshold, loThreshold, parameters.bandCount);
  return hiThreshold;
}

float Processor::incrementVolPeak(int dir)
{
  float step = 0.05;
  volPeak += step * dir;
  if (volPeak < volFloor)
    volPeak = volFloor;
  volActual = volPeak;
  return volPeak;
}

int Processor::incrementLoNote(int dir)
{
  int note = nFirst + dir;
  note = constrain(note, 20, 50);
  nFirst = note;
  buildBins2();
  return nFirst;
}

int Processor::incrementHiNote(int dir)
{
  int note = nLast + dir;
  note = constrain(note, nFirst + parameters.bandCount, 100);
  nLast = note;
  buildBins();
  return nLast;
}

int Processor::incrementNPB(int dir)
{
  int temp = notesPerBand + dir;
  temp = constrain(temp, 1, 5);
  notesPerBand = temp;
  buildBins2();
  return notesPerBand;
}

void Processor::buildBins()
{ // can count beyond samples/2 so needs some fixing
  float C_0 = 16.3515978312874;
  float sampleRate = parameters.sampleRate;
  float res = sampleRate / (1.0f * parameters.sampleCount);
  int minBins = 1;


  // find the starting conditions
  float fnFirst = C_0 * powf(2, nFirst / 12.0f); // determine the frequency of the first requested note
  int bnFirst = fnFirst / res;                   // determine the bin it fits in
  if (bnFirst < 1)
    bnFirst = 1;                     // if that bin is lower than bin 1, set to bin 1
  float fFirst = bnFirst * res;      // determine the frequency of that bin
  // nFirst = 12 * log2f(fFirst / C_0); // convert it to a note

  // find the ending conditions
  float fnLast = C_0 * powf(2, nLast / 12.0f);
  float fsLast = sampleRate / 2;
  float fLast = fnLast;
  if (fsLast < fLast)
    fLast = fsLast;
  int bLast = fLast / res + 1;
  fLast = bLast * res;
  // nLast = 12 * log2f(fLast / C_0);
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

int Processor::buildBins2()
{
    int nMin = nFirst;
    int nMax = notesPerBand * parameters.bandCount + nMin - 1;
    float fMin = C0 * powf(2, (float)nMin / 12);
    float fMax = C0 * powf(2, ((float)nMax + 0.5) / 12);
    

    float bwLo = C0 * (powf(2, (nMin + 0.5) / 12) - powf(2, (nMin - 0.5) / 12));
    float bwHi = (2 * fMax * osrMin) / parameters.sampleCount;
    float bw = bwLo;
    if(bwHi > bwLo) bw = bwHi;

    parameters.sampleRate = round(bw * parameters.sampleCount);

    int n1 = nMin;
    int n2;
    float f1 = C0 * powf(2, (n1 - 0.5) / 12);
    float f2;
    int b1 = (int)(f1 / bw);
    int b2;
    int bc;

    firstBin = b1;

    for (int i = 0; i < parameters.bandCount; i++){
        n2 = n1 + notesPerBand - 1;
        f2 = C0 * powf(2, (n2 + 0.5) / 12);
        b2 = f2 / bw;
        bc = b2 - b1 + 1;

        parameters.binsPerBand[i] = bc;

        n1 = n2 + 1;
        f1 = C0 * powf(2, (n1 - 0.5) / 12);
        b1 = b2 + 1;
    }

    return parameters.sampleRate;
}

void Processor::go()
{
  yAxis();
  binsToBands();
  scale();
}