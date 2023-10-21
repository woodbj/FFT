
void applyWindow() {
  if((int)uvWINDOW == RECTANGULAR) return;
  for (int i = 0; i < SAMPLES; i++) {
    vRe[i] *= wj[(int)uvWINDOW][i];
  }
}

void pushBuffer() {
  int currentIndex = bufferIndex; // get the index the buffer is currently filling to
  int firstIndex = currentIndex - SAMPLES + 1;  // go back by the number of samples being taken
  if (firstIndex < 0) firstIndex += SAMPLES;  // if that's < 0 loop it back
  
  int ind = firstIndex;
  for (int i = 0; i < SAMPLES; i++) {
    ind = (firstIndex + i) % bufferLen;
    vRe[i] = buffer[ind];
    vIm[i] = 0;
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


