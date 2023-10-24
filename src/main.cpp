#include "header.h"

void Compute(void *)
{
  Queue_Message_t message = READY_TO_PROCESS;
  for (;;)
  {
    capFPS(25);
    mic.getBuffer(samples);
    computeSamples();
    xQueueSend(queue, &message, portMAX_DELAY);
  }
}

void ProcessTask(void *)
{
  Queue_Message_t message;
  unsigned long tStart;
  for (;;)
  {
    if (xQueueReceive(queue, &message, portMAX_DELAY) == pdTRUE && message == READY_TO_PROCESS)
    {
      processor.go();
      displaySamples();
    }
  }
}

void setup()
{

  queue = xQueueCreate(1, sizeof(Queue_Message_t));

  // set up LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LED);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
  FastLED.setBrightness(100);

  // set up serial
  Serial.begin(115200);

  // Pre-fill the integral array so averaging isn't way up the fuck at the start

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



  xTaskCreatePinnedToCore(Compute, "Compute Task", STACK_SIZE, nullptr, 1, &computer, 1);
  xTaskCreatePinnedToCore(ProcessTask, "Process Task", STACK_SIZE, nullptr, 1, &processtask, 0);
}

void loop()
{
  delay(portMAX_DELAY);
}
