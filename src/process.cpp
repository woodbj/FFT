#include <process.h>
#include <Arduino.h>

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Processor::scale()
{
  float val;              // holds the band value while editing
  float volChange = 0.01; // how much to change the volume each iteration
  float volMin = 0.1;     // hold the min value observed
  float average = 0;
  float min = 1e6;
  float max = 0;
  float span;
  float delta;
  float target;

  for (int band = 0; band < parameters.bandCount; band++)
  {
    // get the max and min
    val = parameters.bandValues[band];
    if (val < min)
      min = val;
    if (val > max)
      max = val;

    // scale
    val *= volActual;
    if (val > 1.0)
      val = 1.0;
    else if (val < 0.0)
      val = 0.0;

    // get the average
    average += val;

    // add non-linearity to the response
    val = powf(val, gain);

    parameters.bandValues[band] = val;
  }

  // calculate the average
  average /= parameters.bandCount;

  // calculate the min-max span and add nonlinearity so "quiet" can be identified
  span = max - min;
  span = powf(5, span);

  if (span > 3)
  { // if it's loud
    target = hiThreshold;
  }
  else
  { // if it's quiet
    target = 0.05;
  }

  delta = target - average;
  volActual += delta * volChange;

  if (volActual < volMin)
    volActual = volMin;
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

float Processor::incrementGain(int dir)
{
  double min = 1;
  double max = 5;
  double step = 0.5 * dir;

  gain += step;
  if (gain < min)
    gain = min;
  if (gain > max)
    gain = max;

  return gain;
}

float Processor::incrementVolTarget(int dir)
{
  hiThreshold += 0.05 * dir;
  hiThreshold = constrain(hiThreshold, 0, 1);
  return hiThreshold;
}

int Processor::incrementLoNote(int dir)
{
  int old = nFirst;
  int note = nFirst + constrain(dir, -1, 1);
  note = constrain(note, 30, 50);
  nFirst = note;
  if (dir != 0)
  {
    Serial.printf("\nIncrement n1 by %d", dir);
    if (buildBins() != 0)
    {
      Serial.printf("\nIncrement fail");
      nFirst = old;
      buildBins();
    }
  }
  return nFirst;
}

int Processor::incrementNPB(int dir)
{
  int old = notesPerBand;
  int temp = notesPerBand + constrain(dir, -1, 1);
  temp = constrain(temp, 1, 3);
  notesPerBand = temp;
  if (dir != 0)
  {
    Serial.printf("\nIncrement NPB by %d", dir);
    if (buildBins() != 0)
    {
      Serial.printf("\nIncrement fail");
      notesPerBand = old;
      buildBins();
    }
  }
  return notesPerBand;
}

int Processor::buildBins()
{
  Serial.printf("\nBuilding Bins");
  float offset = 0.5;
  int nMin = nFirst;
  int nMax = notesPerBand * parameters.bandCount + nMin - 1;
  float fMin = C0 * powf(2, (float)nMin / 12);
  float fMax = C0 * powf(2, ((float)nMax + offset) / 12);

  float bwLo = C0 * (powf(2, (nMin + 0.5) / 12) - powf(2, (nMin - 0.5) / 12));
  float bwHi = (2 * fMax) / parameters.sampleCount;
  float bw = bwLo;
  if (bwHi > bwLo)
  {
    bw = bwHi;
  }

  Serial.printf("\n requested sr = %.0f", round(bw * parameters.sampleCount));

  parameters.sampleRate = round((bw * parameters.sampleCount));
  Serial.printf("\n new sample rate %d", parameters.sampleRate);

  int n1 = nMin;
  int n2;
  float f1 = C0 * powf(2, (n1 - offset) / 12);
  float f2;
  int b1 = (int)(f1 / bw);
  int b2;
  int bc;
  int bsum = 0;
  bool redo = false;

  firstBin = b1;

  Serial.printf("\n first bin = %d", b1);

  Serial.printf("\n bin:");

  for (int i = 0; i < parameters.bandCount; i++)
  {
    n2 = n1 + notesPerBand - 1;
    f2 = C0 * powf(2, (n2 + offset) / 12);
    b2 = f2 / bw;
    bc = b2 - b1 + 1;
    bsum += bc;

    Serial.printf("\nbins %d - %d, count %d", b1, b2, bc);

    if (bc == 0)
    {
      Serial.printf("\n redoing, bin count = 0");
    }

    parameters.binsPerBand[i] = bc;
    n1 = n2 + 1;
    f1 = C0 * powf(2, (n1 - offset) / 12);
    b1 = b2 + 1;
  }

  lastBin = bsum + firstBin - 1;

  Serial.printf("\n last bin = %d", lastBin);

  if (redo)
  {
    return -1;
  }

  return 0;
}

void Processor::go()
{
  binsToBands();
  scale();
}