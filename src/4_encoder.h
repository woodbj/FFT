
void swISR() {
  if (millis() - debounceLastTime < debounce_ms) return;
  debounceLastTime = millis();

  swPress = true;
}

void enISR() {
  bool dt = digitalRead(DT);
  bool clk = digitalRead(CLK);

  // debouncing
  if (millis() - debounceLastTime < debounce_ms) return;
  debounceLastTime = millis();

  // determine fast or slow turning
  if (millis() - lastClick < 15) {
    fastTurn = true;
  } else {
    fastTurn = false;
  }
  lastClick = millis();

  // determine direction
  if (clk == dt) {
    enIncrement = true;
    enCounter++;
  } else {
    enCounter--;
    enDecrement = true;
  }
}
