#include <process.h>
#include <Arduino.h>

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Processor::scale()
{
  Serial.printf("\n");
  // Scale the bandValues
  float val;
  int counter = 0;
  float volDelta = 0.001;
  float volMin = 0.1;
  float average = 0;
  float min = 1e6;
  float max = 0;
  float floor = 0.2;
  float avgScaled;
  float span;
  int delta;

  for (int band = 0; band < parameters.bandCount; band++)
  {
    // get the max and min
    val = parameters.bandValues[band];
    if (val < min)
      min = val;
    if (val > max)
      max = val;

    val = powf(val, gain); // add non-linearity to the response
    val *= volActual;      // scale
    if (val > 1.0)
      val = 1.0; // set peak value at 1
    parameters.bandValues[band] = val;
    if (val == 1)
      counter++;

    average += val;
  }

  average /= parameters.bandCount;
  avgScaled = fmap(average, min, max, 0, 1);
  span = max - min;
  span = powf(10, span);

  delta = counter - hiThreshold;
  Serial.printf("\t%d", delta);

  if (span > 8)
  { // if there's stuff playing, target the set count of max values
    volActual += (-delta) * volDelta;
  }
  else
  { // if it's quiet, target a pixel value
    if(average > 0.1)
    {
      volActual -= volDelta;
    }
    else
    {
      volActual += volDelta;
    }
  }

  if(volActual <= 0) volActual = volMin;

  Serial.printf("\t%f", volActual);
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

  float res = parameters.sampleRate / (1.0f * parameters.sampleCount);
  float bandf;
  int n1;
  int n2;
  float navg;

  // fill bands and integral
  int startBin = firstBin;
  for (int band = 0; band < parameters.bandCount; band++)
  {
    parameters.bandValues[band] = 0;

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

    n1 = nFirst + band * notesPerBand;
    n2 = n1 + notesPerBand - 1;
    navg = ((float)n1 + (float)n2) / 2.0f;
    bandf = noteToFreq(navg);
    parameters.bandValues[band] = parameters.bandValues[band] / sqrt(bandf);
  }
}

double Processor::incrementGain(int dir)
{
  double min = 1;
  double max = 10;
  double step = 1 * dir;

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

float Processor::incrementOSR(int dir)
{
  float temp = osrMin;
  temp += 0.1 * dir;
  if(temp < 1) temp = 1;
  osrMin = temp;
  buildBins();
  return osrMin;
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
  int note = nFirst + constrain(dir, -1, 1);
  note = constrain(note, 30, 50);
  nFirst = note;
  buildBins();
  return nFirst;
}

int Processor::incrementNPB(int dir)
{
  int temp = notesPerBand + constrain(dir, -1, 1);
  temp = constrain(temp, 1, 3);
  notesPerBand = temp;
  buildBins();
  return notesPerBand;
}

int Processor::buildBins()
{
  float offset = 0;
  int nMin = nFirst;
  int nMax = notesPerBand * parameters.bandCount + nMin - 1;
  float fMin = C0 * powf(2, (float)nMin / 12);
  float fMax = C0 * powf(2, ((float)nMax + offset) / 12);

  float bwLo = C0 * (powf(2, (nMin + offset) / 12) - powf(2, (nMin - offset) / 12));
  float bwHi = (2 * fMax * osrMin) / parameters.sampleCount;
  float bw = bwLo;
  if (bwHi > bwLo)
    bw = bwHi;

  parameters.sampleRate = round(bw * parameters.sampleCount);

  int n1 = nMin;
  int n2;
  float f1 = C0 * powf(2, (n1 - offset) / 12);
  float f2;
  int b1 = (int)(f1 / bw);
  int b2;
  int bc;
  int bsum = 0;

  firstBin = b1;

  for (int i = 0; i < parameters.bandCount; i++)
  {
    n2 = n1 + notesPerBand - 1;
    f2 = C0 * powf(2, (n2 + offset) / 12);
    b2 = f2 / bw;
    bc = b2 - b1 + 1;
    bsum += bc;

    parameters.binsPerBand[i] = bc;

    n1 = n2 + 1;
    f1 = C0 * powf(2, (n1 - offset) / 12);
    b1 = b2 + 1;
  }

  lastBin = bsum + firstBin - 1;

  return parameters.sampleRate;
}

void Processor::go()
{
  binsToBands();
  scale();
}