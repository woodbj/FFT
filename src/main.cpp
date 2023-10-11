#include "header.h"

void Sample(void *)
{
  unsigned int newTime;
  unsigned int endTime;

  for (;;)
  {
    newTime = micros();
    buffer[bufferIndex] = adc1_get_raw((adc1_channel_t)MIC_PIN);
    bufferIndex++;
    bufferIndex %= bufferLen;
    while ((micros() - newTime) < sampling_period_us)
    {
    }; // chill
    if (bufferIndex == 0)
      delay(1); // // keep the watchdog happy
  }
}

void Compute(void *)
{
  attachInterrupt(SW, swISR, RISING);
  attachInterrupt(CLK, enISR, CHANGE);
  unsigned long tStart;
  unsigned int displayPeriod;
  float displayHz;
  unsigned int actualPeriod;
  int colourMode = 0;
  delay(250);
  for (;;)
  {

    tStart = micros();
    computeSamples();
    processSamples();
    menu();
    displaySamples();

    delay(1); // keep the watchdog happy

    displayPeriod = SAMPLES * (1e6 / uv[SAMPLERATE].val) * (1 - uv[OVERLAP].val / 100.0f);

    while (micros() - tStart < displayPeriod){
      Serial.println("Waiting");
    } // chill

    // displayHz = 1.0e6 / (micros() - tStart);
    // Serial.println(displayHz);

    // for some reason the frame rate is increasing as sample rate decreases
  }
}

void setup()
{

  // fill user variables
  strcpy(uv[VOLUME].title, "LOUD");
  uv[VOLUME].val = 2;
  uv[VOLUME].min = 0.5;
  uv[VOLUME].max = 10;
  uv[VOLUME].delta = 0.01;
  uv[VOLUME].fastEnable = true;
  uv[VOLUME].fastDelta = 0.5;

  strcpy(uv[GAIN].title, "GAIN");
  uv[GAIN].val = 1;
  uv[GAIN].delta = 0.1;
  uv[GAIN].min = 0.5;
  uv[GAIN].max = 1.5;

  strcpy(uv[WINDOW].title, "WIN");
  uv[WINDOW].val = HANNING;
  uv[WINDOW].min = 0;
  uv[WINDOW].max = WINDOW_COUNT - 1;
  uv[WINDOW].delta = 1;

  strcpy(uv[OVERLAP].title, "OVLP");
  uv[OVERLAP].val = 50;
  uv[OVERLAP].min = 0;
  uv[OVERLAP].max = 50;
  uv[OVERLAP].delta = 1;
  uv[OVERLAP].fastEnable = true;
  uv[OVERLAP].fastDelta = 5;

  strcpy(uv[SAMPLERATE].title, "SRAT");
  uv[SAMPLERATE].val = 35000;
  uv[SAMPLERATE].min = 15000;
  uv[SAMPLERATE].max = 35000;
  uv[SAMPLERATE].delta = 50;
  uv[SAMPLERATE].fastEnable = true;
  uv[SAMPLERATE].fastDelta = 1000;

  strcpy(uv[FLOW].title, "LO");
  uv[FLOW].val = 32;
  uv[FLOW].min = 32;
  uv[FLOW].max = 48;
  uv[FLOW].delta = 1;

  strcpy(uv[FHIGH].title, "HI");
  uv[FHIGH].val = 80;
  uv[FHIGH].min = 60;
  uv[FHIGH].max = 117;
  uv[FHIGH].delta = 1;
  uv[FHIGH].fastEnable = true;
  uv[FHIGH].fastDelta = 5;
  uv[FHIGH].reverseValueBar = true;

  strcpy(uv[PEAKS].title, "PEAK");
  uv[PEAKS].val = 1;
  uv[PEAKS].min = 0;
  uv[PEAKS].max = 1;
  uv[PEAKS].delta = 0.25;

  strcpy(uv[YSCALE].title, "YSCL");
  uv[YSCALE].val = INT_POWER_DENSITY;
  uv[YSCALE].min = 0;
  uv[YSCALE].max = LAST_YSCALE - 1;
  uv[YSCALE].delta = 1;

  strcpy(uv[STYLE].title, "STYL");
  uv[STYLE].val = SPECTROGRAM;
  uv[STYLE].min = SPECTROGRAM;
  uv[STYLE].max = LAST_STYLE - 1;
  uv[STYLE].delta = 1;
  uv[STYLE].rollover = false;

  strcpy(styleSettings[SPECTROGRAM].title, "PALT");
  styleSettings[SPECTROGRAM].val = 0;
  styleSettings[SPECTROGRAM].min = 0;
  styleSettings[SPECTROGRAM].max = PAL_COUNT - 1;
  styleSettings[SPECTROGRAM].delta = 1;
  styleSettings[SPECTROGRAM].showValueBar = true;

  strcpy(styleSettings[COLOUR_BC].title, "COLR");
  styleSettings[COLOUR_BC].val = 0;
  styleSettings[COLOUR_BC].min = 0;
  styleSettings[COLOUR_BC].max = 360;
  styleSettings[COLOUR_BC].delta = 1;
  styleSettings[COLOUR_BC].fastEnable = true;
  styleSettings[COLOUR_BC].fastDelta = 10;
  styleSettings[COLOUR_BC].rollover = true;
  styleSettings[COLOUR_BC].showValueBar = false;
  styleSettings[COLOUR_BCW] = styleSettings[COLOUR_BC];

  strcpy(styleSettings[RAINBOW_BC].title, "RATE");
  styleSettings[RAINBOW_BC].val = 0;
  styleSettings[RAINBOW_BC].min = 0;
  styleSettings[RAINBOW_BC].max = 50;
  styleSettings[RAINBOW_BC].delta = 1;
  styleSettings[RAINBOW_BC].fastEnable = true;
  styleSettings[RAINBOW_BC].fastDelta = 10;
  styleSettings[RAINBOW_BC].rollover = false;
  styleSettings[RAINBOW_BC].showValueBar = true;
  styleSettings[RAINBOW_BCW] = styleSettings[RAINBOW_BC];

  uv[STYLE_SETTING] = styleSettings[(int)uv[STYLE].val];

  // set up LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LED);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
  FastLED.setBrightness(100);

  // set up serial
  Serial.begin(115200);

  // set up pins
  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(MIC_PIN, ADC_ATTEN_11db);
  pinMode(MIC_PIN, INPUT);
  pinMode(CLK, INPUT);
  pinMode(SW, INPUT);
  pinMode(DT, INPUT);

  // Pre-fill the integral array so averaging isn't way up the fuck at the start
  for (int i = 0; i < avgSamples; i++)
  {
    integral[i] = 0.25;
  }

  for (int band = 0; band < MAT_W; band++)
  {
    bandValues[band] = 0;
  }

  for (int led = 0; led < NUM_LED; led++)
  {
    spect[led] = CRGB::Black;
  }

  for (int i = 0; i < SAMPLES; i++)
  {
    float z = TWOPI * i / (SAMPLES * 1.0f);
    wj[RECTANGULAR][i] = 1;
    wj[HANNING][i] = (1 - cos(z)) / 2.0f;
    wj[HAMMING][i] = (0.54 - 0.46 * cos(z));
    wj[TRIANGULAR][i] = 1 - abs((i - SAMPLES / 2.0f) / (SAMPLES / 2.0f));
    wj[SINE][i] = sin(z / 2.0f);
  }

  applyChanges();

  // assign tasks to cores
  xTaskCreatePinnedToCore(Sample, "Sample Task", STACK_SIZE, nullptr, 1, &sampler, 0);
  xTaskCreatePinnedToCore(Compute, "Compute Task", STACK_SIZE, nullptr, 1, &computer, 1);
}

void loop()
{
  delay(portMAX_DELAY);
}
