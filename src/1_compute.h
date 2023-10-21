
void applyWindow() {
  if((int)uvWINDOW == RECTANGULAR) return;
  for (int i = 0; i < SAMPLES; i++) {
    // vRe[i] *= wj[(int)uvWINDOW][i];
    vRe[i] *= wj[HAMMING][i];
  }
}

void prepareInput(){
  for (int i = 0; i < SAMPLES; i++){
    vRe[i] = samples[i];
    vIm[i] = 0;
  }
}

void getMagnitude() {
  float real;
  float imag;
  float pythag;
  float invSamples = 1.0f / SAMPLES;

  for (int i = 0; i < SAMPLES / 2; i++) {
    pythag = vRe[i] * vRe[i] + vIm[i] * vIm[i];
    vRe[i] = sqrt(pythag) * invSamples;
  }
}

void computeSamples() {
  prepareInput();
  applyWindow();
  FFT.Compute(FFT_FORWARD);
  getMagnitude();
}


