#include "header.h"

void Compute(void *)
{
  unsigned long tStart = millis();
  unsigned int displayPeriod;
  int fat = 1000;
  Queue_Message_t message = READY_TO_PROCESS;
  for (;;)
  {
    mic.getBuffer(samples);
    computeSamples();
    xQueueSend(queue, &message, portMAX_DELAY);
    delay(1); // keep the watchdog happy
  }
}

void Process(void *)
{
  attachInterrupt(SW, swISR, RISING);
  attachInterrupt(CLK, enISR, CHANGE);
  Queue_Message_t message;
  unsigned long tStart;
  for (;;)
  {
    if (xQueueReceive(queue, &message, portMAX_DELAY) == pdTRUE && message == READY_TO_PROCESS)
    {
      tStart = micros();
      processSamples();
      menu();
      displaySamples();
    }
  }
}

void setup()
{

  queue = xQueueCreate(1, sizeof(Queue_Message_t));
  // fill user variables
  strcpy(uv[VOLUME].title, "LOUD");
  uv[VOLUME].ptr = &uvVOLUME;
  uv[VOLUME].min = 0.1;
  uv[VOLUME].max = 10;
  uv[VOLUME].delta = 1;

  strcpy(uv[GAIN].title, "GAIN");
  uv[GAIN].ptr = &uvGAIN;
  uv[GAIN].delta = 0.1;
  uv[GAIN].min = 0.5;
  uv[GAIN].max = 1.5;

  strcpy(uv[FLOW].title, "LO");
  uv[FLOW].ptr = &uvFLOW;
  uv[FLOW].min = 32;
  uv[FLOW].max = 48;
  uv[FLOW].delta = 1;

  strcpy(uv[FHIGH].title, "HI");
  uv[FHIGH].ptr = &uvFHIGH;
  uv[FHIGH].min = 60;
  uv[FHIGH].max = 117;
  uv[FHIGH].delta = 1;
  uv[FHIGH].fastEnable = true;
  uv[FHIGH].fastDelta = 5;
  uv[FHIGH].reverseValueBar = true;

  strcpy(uv[YSCALE].title, "YSCL");
  uv[YSCALE].ptr = &uvYSCALE;
  uv[YSCALE].min = 0;
  uv[YSCALE].max = LAST_YSCALE - 1;
  uv[YSCALE].delta = 1;

  strcpy(uv[STYLE].title, "STYL");
  uv[STYLE].ptr = &uvSTYLE;
  uv[STYLE].min = SPECTROGRAM;
  uv[STYLE].max = LAST_STYLE - 1;
  uv[STYLE].delta = 1;
  uv[STYLE].rollover = false;

  strcpy(styleSettings[SPECTROGRAM].title, "PALT");
  styleSettings[SPECTROGRAM].ptr = &uvSTYLE_SETTING_SPECTROGRAM;
  styleSettings[SPECTROGRAM].min = 0;
  styleSettings[SPECTROGRAM].max = PAL_COUNT - 1;
  styleSettings[SPECTROGRAM].delta = 1;
  styleSettings[SPECTROGRAM].showValueBar = true;

  strcpy(styleSettings[COLOUR_BC].title, "COLR");
  styleSettings[COLOUR_BC].ptr = &uvSTYLE_SETTING_COLOUR_BC;
  styleSettings[COLOUR_BC].min = 0;
  styleSettings[COLOUR_BC].max = 360;
  styleSettings[COLOUR_BC].delta = 1;
  styleSettings[COLOUR_BC].fastEnable = true;
  styleSettings[COLOUR_BC].fastDelta = 10;
  styleSettings[COLOUR_BC].rollover = true;
  styleSettings[COLOUR_BC].showValueBar = false;
  styleSettings[COLOUR_BCW] = styleSettings[COLOUR_BC];

  strcpy(styleSettings[RAINBOW_BC].title, "RATE");
  styleSettings[RAINBOW_BC].ptr = &uvSTYLE_SETTING_RAINBOW_BC;
  styleSettings[RAINBOW_BC].min = 0;
  styleSettings[RAINBOW_BC].max = 50;
  styleSettings[RAINBOW_BC].delta = 1;
  styleSettings[RAINBOW_BC].fastEnable = true;
  styleSettings[RAINBOW_BC].fastDelta = 10;
  styleSettings[RAINBOW_BC].rollover = false;
  styleSettings[RAINBOW_BC].showValueBar = true;
  styleSettings[RAINBOW_BCW] = styleSettings[RAINBOW_BC];

  uv[STYLE_SETTING] = styleSettings[(int)uv[STYLE].val];

  buildBins();

  // set up LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LED);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
  FastLED.setBrightness(100);

  // set up serial
  Serial.begin(115200);

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

  Mic_Settings_t mic_settings;
  mic_settings.ws = I2S_WS;
  mic_settings.sck = I2S_SCK;
  mic_settings.sd = I2S_SD;
  mic_settings.sample_rate = SAMPLE_FREQ;
  mic_settings.sample_count = SAMPLES;

  mic.begin(mic_settings);

  pinMode(CLK, INPUT);
  pinMode(SW, INPUT);
  pinMode(DT, INPUT);

  xTaskCreatePinnedToCore(Compute, "Compute Task", STACK_SIZE, nullptr, 1, &computer, 1);
  xTaskCreatePinnedToCore(Process, "Process Task", STACK_SIZE, nullptr, 1, &processor, 0);
}

void loop()
{
  delay(portMAX_DELAY);
}
