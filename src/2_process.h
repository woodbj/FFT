

void getPeaks(){
  for (int i = 2; i < SAMPLES / 2 - 1; i++){
    if(vRe[i] > vRe[i - 1] && vRe[i] > vRe[i + 1]);
    else vRe[i] *= peakWeights;
  }
}

void yAxis() {
  float res = uv[SAMPLERATE].val / (1.0f * SAMPLES);
  float binf;
  int scaleCase = (int)uv[YSCALE].val;

  for (int i = 1; i <= SAMPLES / 2; i++) {
    switch (scaleCase) {
      case DB_POWER_DENSITY:
        vRe[i] = 20 * log10f(vRe[i]);
        binf = i * res;
        if(vRe[i] < 0) vRe[i] = 0;
        vRe[i] = vRe[i] / sqrt(binf);
        break;
      case INT_POWER_DENSITY:
        binf = i * res;
        if(vRe[i] < 0) vRe[i] = 0;
        vRe[i] = 1.25 * vRe[i] / sqrt(binf);
        break;
    }
  }
}

void scale() {
  // Scale the bandValues
  float val;
  int counter = 0;
  for (int band = 0; band < MAT_W; band++) {
    val = bandValues[band];
    val = powf(val, uv[GAIN].val);  // add non-linearity to the response
    val *= uv[VOLUME].val;          // scale
    if (val > 1.0) val = 1.0;       // set peak value at 1
    bandValues[band] = val;
    if (val == 1) counter++;
  }

  if (counter > 10) {
    uv[VOLUME].val -= uv[VOLUME].delta;
  } else if (counter < 2) {
    uv[VOLUME].val += uv[VOLUME].delta;
  }
  uv[VOLUME].val = constrain(uv[VOLUME].val, uv[VOLUME].min, uv[VOLUME].max);
}

void binsToBands() {

  // zero out the bands for filling
  for (int i = 0; i < MAT_W; i++) {
    bandValues[i] = 0;
  }
  integral[avgCounter] = 0;

  // fill bands and integral
  int startBin = firstBin;
  for (int band = 0; band < MAT_W; band++) {

    // add each applicable bin to respective bands
    float val;
    int endBin = startBin + binsPerBand[band] - 1;
    for (int bin = startBin; bin <= endBin; bin++) {
      val = vRe[bin];
      // val = log10(val + 1);
      bandValues[band] += val;
      integral[avgCounter] += val;
    }
    startBin = endBin + 1;
  }

  avgCounter++;
  if (avgCounter >= avgSamples) avgCounter = 0;
}

float freqToNote(float f) {
  return 12 * log2f(f / 16.3515978312874);
}

float noteToFreq(float n) {
  return 16.3515978312874 * powf(2, n / 12.0);
}

void buildBins() {  // can count beyond samples/2 so needs some fixing
  float C_0 = 16.3515978312874;
  float sampleRate = uv[SAMPLERATE].val;
  float res = sampleRate / (1.0f * SAMPLES);
  int minBins = 1;
  float nFirst = uv[FLOW].val;
  float nLast = uv[FHIGH].val;

  // find the starting conditions
  float fnFirst = C_0 * powf(2, nFirst / 12.0f);  // determine the frequency of the first requested note
  int bnFirst = fnFirst / res;                    // determine the bin it fits in
  if (bnFirst < 1) bnFirst = 1;                      // if that bin is lower than bin 1, set to bin 1
  float fFirst = bnFirst * res;       // determine the frequency of that bin
  nFirst = 12 * log2f(fFirst / C_0);  // convert it to a note

  // find the ending conditions
  float fnLast = C_0 * powf(2, nLast / 12.0f);
  float fsLast = sampleRate / 2;
  float fLast = fnLast;
  if (fsLast < fLast) fLast = fsLast;
  int bLast = fLast / res + 1;
  fLast = bLast * res;
  nLast = 12 * log2f(fLast / C_0);
  // find the notes per band
  float nPerBand = (nLast - nFirst) / MAT_W;
  float nStart = nFirst;
  float fStart = fFirst;
  int bStart = bnFirst;
  int bEnd;
  float fEnd;
  float nEnd;
  int bCount;
  firstBin = bStart;
  for (int band = 0; band < MAT_W; band++) {
    // first bStart before loop, subsequent at the end of the loop
    fStart = res * bStart;
    // nStart = freqToNote(fStart);
    nEnd = nStart + nPerBand;
    fEnd = noteToFreq(nEnd);
    bEnd = fEnd / res;
    bCount = bEnd - bStart + 1;
    if (bCount < minBins) bCount = minBins;
    bEnd = bStart + bCount - 1;
    if (band == MAT_W - 1) {
      while (bEnd > SAMPLES / 2) {
        bCount--;
        bEnd--;
      }
    }
    binsPerBand[band] = bCount;
    bStart = bEnd + 1;
    nStart = nEnd;
  }
}



void processSamples() {
  getPeaks();
  yAxis();
  binsToBands();
  scale();
}