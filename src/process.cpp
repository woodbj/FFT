#include <process.h>
#include <Arduino.h>

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

Processor::Processor(Processor_Parameters_t p)
{
  parameters = p;
  buildBins();
}

void Processor::setSampleRate(int newSampleRate)
{
  parameters.sampleRate = newSampleRate;
  if (!ignoreSRUpdate)
  {
    buildBins();
  }
  ignoreSRUpdate = false;
}

void Processor::binsToBands()
{

  int currentBand = 0;
  float binNote;
  float bw = parameters.sampleRate / parameters.sampleCount;
  float n1 = nFirst - 0.5;
  float n2 = n1 + notesPerBand;
  parameters.bandValues[currentBand] = 0;
  int binCount = 0;
  float val;

  stats.currentMax = 0;
  stats.currentMin = __FLT_MAX__;
  stats.currentAvg = 0;

  for (int i = 2; i < parameters.sampleCount / 2 - 1; i++)
  {
    binNote = freqToNote(bw * i);

    if (binNote >= n1 && binNote < n2)
    {
      val = parameters.vRe[i];
      parameters.bandValues[currentBand] += val;
      stats.currentAvg += val;
      if (val > stats.currentMax)
        stats.currentMax = val;
      else if (val < stats.currentMin && val != 0)
        stats.currentMin = val;

      if (val > stats.allTimeMax && val < 500)
        stats.allTimeMax = val;
      else if (val < stats.allTimeMin && val != 0)
        stats.allTimeMin = val;

      binCount++;
    }
    else if (binNote > n2)
    {

      currentBand++;
      if (currentBand >= parameters.bandCount)
        break;

      parameters.bandValues[currentBand] = 0;

      n1 = n2;
      n2 += notesPerBand;

      i--;
    }
  }

  float span = stats.currentMax - stats.currentMin;
  stats.currentAvg = stats.currentAvg / binCount;
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

float Processor::incrementDBFSFloor(int dir)
{
  float val = dbfsFloor - dir;
  if (val < dbfsCeiling)
    dbfsFloor = val;
  return dbfsFloor;
}

float Processor::incrementDBFSCeiling(int dir)
{
  float val = dbfsCeiling - dir;
  if (val > dbfsFloor)
    dbfsCeiling = val;
  return dbfsCeiling;
}

int Processor::incrementVolMode(int dir)
{
  volMode = constrain(volMode + dir, 0, LAST_VOL_MODE - 1);
  return volMode;
}

float Processor::incrementVolume(int dir)
{
  volActual = volActual + 0.001 * dir;
  if (volActual < 0)
    volActual = 0;

  return volActual;
}

int Processor::incrementLoNote(int dir)
{
  int old = nFirst;
  int note = nFirst + constrain(dir, -1, 1);
  note = constrain(note, 30, 50);
  nFirst = note;
  if (dir != 0)
  {
    if (buildBins() != 0)
    {
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
  temp = constrain(temp, 1, 4);
  notesPerBand = temp;
  if (dir != 0)
  {
    if (buildBins() != 0)
    {
      notesPerBand = old;
      buildBins();
    }
  }
  return notesPerBand;
}

void Processor::resetStats()
{
  stats.allTimeMax = 0;
  stats.allTimeMin = __FLT_MAX__;
}

int Processor::buildBins()
{
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

  parameters.sampleRate = round((float)(bw * parameters.sampleCount) / 100) * 100;

  *parameters.newSR = parameters.sampleRate;
  *parameters.resetSR = true;
  ignoreSRUpdate = true;

  resetStats();

  return 0;
}

void Processor::rawToDPFS()
{
  float val;
  float ref = pow(2, 15);
  float nlo;
  float nhi;
  float bandf;
  static float sumlast = 0;
  float weight = 0.001;
  float sum = 8;
  bool fscale = true;

  Serial.printf("\n");
  for (int i = 0; i < parameters.bandCount; i++)
  {
    val = parameters.bandValues[i];
    nlo = nFirst + i * notesPerBand - 0.5;
    nhi = nlo + notesPerBand;

    if (fscale)
    {
      bandf = noteToFreq((nlo + nhi) / 2);
      val /= sqrt(bandf);
    }


    val = 20 * log10(val / ref);

    val = fmap(val, dbfsFloor, dbfsCeiling, 0, 1);
    val = constrain(val, 0, 1);

    sum += val;

    val = powf(val, gain);
    parameters.bandValues[i] = val;
  }
  sum = sum * weight + sumlast * (1 - weight);
  sumlast = sum;
  Serial.printf("\t%f", sum);
}

void Processor::scale()
{
  float val;              // holds the band value while editing
  float volChange = 0.01; // how much to change the volume each iteration
  float volMin = 0.01;    // hold the min value observed
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
  span = powf(10, span);
  if (span > 1000)
    span = 1000;

  if (span > 3)
  { // if it's loud
    target = hiThreshold;
  }
  else
  { // if it's quiet
    target = 0.05;
  }

  delta = target - average;
  if (volMode == VolumeMode::AUTO)
  {
    volActual += delta * volChange;
  }

  if (volActual < volMin)
    volActual = volMin;
}

void Processor::go()
{
  binsToBands();
  rawToDPFS();
  // scale();
}