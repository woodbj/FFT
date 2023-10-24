#include "header.h"

void Compute(void *)
{
  Queue_Message_t message = READY_TO_PROCESS;
  for (;;)
  {
    capFPS(25);
    mic.getBuffer(samples);
    fft.go(samples);
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

  

  // set up LEDs
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LED);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1000);
  FastLED.setBrightness(100);

  // set up serial
  Serial.begin(115200);

  for (int band = 0; band < MAT_W; band++)
  {
    bandValues[band] = 0;
  }

  for (int led = 0; led < NUM_LED; led++)
  {
    spect[led] = CRGB::Black;
  }

  queue = xQueueCreate(1, sizeof(Queue_Message_t));

  xTaskCreatePinnedToCore(Compute, "Compute Task", STACK_SIZE, nullptr, 1, &computer, 1);
  xTaskCreatePinnedToCore(ProcessTask, "Process Task", STACK_SIZE, nullptr, 1, &processtask, 0);
}

void loop()
{
  delay(portMAX_DELAY);
}
