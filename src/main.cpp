#include "header.h"

void swISR()
{
  enc.setSwitch();
}

void dialISR()
{
  enc.setDial();
}

void Compute(void *)
{
  Queue_Message_t message = READY_TO_PROCESS;
  for (;;)
  {
    capFPS(fpsRequested);
    
    fft.go(samples);
    xQueueSend(queue, &message, portMAX_DELAY);
  }
}

void ProcessTask(void *)
{
  Queue_Message_t message;
  unsigned long tStart;
  attachInterrupt(ENC_SW, swISR, RISING);
  attachInterrupt(ENC_CLK, dialISR, CHANGE);
  for (;;)
  {
    if (xQueueReceive(queue, &message, portMAX_DELAY) == pdTRUE && message == READY_TO_PROCESS)
    {
      processor.go();
      matrix.go();
      gEncoderState = enc.getState();
      menu.navigate(gEncoderState);
      mic.getBuffer(samples);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  menu.add(changeStyle);
  menu.add(changeStyleParameter);
  menu.add(changeHiVol);
  menu.add(changeNPB);
  menu.add(changeLoNote);
  menu.add(changeFPS);
  

  queue = xQueueCreate(1, sizeof(Queue_Message_t));

  xTaskCreatePinnedToCore(Compute, "Compute Task", STACK_SIZE, nullptr, 1, &computer, 1);
  xTaskCreatePinnedToCore(ProcessTask, "Process Task", STACK_SIZE, nullptr, 1, &processtask, 0);
}

void loop()
{
  delay(portMAX_DELAY);
}
