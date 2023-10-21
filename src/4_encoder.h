


void applyChanges(){
  float localVal;
  float globalVal;
  int intval;
  
  for (int i = 0; i < MENU_COUNT; i++){
    if(uv[i].changed){
      switch(i){
        case FLOW:
          buildBins();
          break;

        case FHIGH:
          buildBins();
          break;

        // case SAMPLERATE:
        //   sampling_period_us = 1e6 / uvSAMPLERATE;
        //   buildBins();
        //   intval = uvSAMPLERATE;
        //   mic.setSampleRate(intval);
        //   break;

        case STYLE:
          switch((int)uvSTYLE){
            case SPECTROGRAM:
              uvYSCALE = INT_POWER_DENSITY;
              uvGAIN = uv[GAIN].max;
              break;

            default: // COLOUR and RAINBOW
              uvYSCALE = DB_POWER_DENSITY;
              uvGAIN = 1;
              break;
          }
      }
      uv[i].changed = false;
    }
  }
}

void editUserVariable(int dir, UserVar_t* var) {

  float delta;

  if (fastTurn && var->fastEnable) delta = var->fastDelta;
  else delta = var->delta;

  var->val += dir * delta;

  if (var->val > var->max) {
    // if value exceeds max
    if (var->rollover) var->val = var->min;
    else var->val = var->max;

  } else if (var->val < var->min) {
    // if value is less than the min
    if (var->rollover) var->val = var->max;
    else var->val = var->min;
  }


  var->changed = true;
  *var->ptr = var->val;


  // tell the display what it needs to know to show the value bar
  currentValue = var->val;
  currentMin = var->min;
  currentMax = var->max;
}

void changeMenu(int dir) {

  if (dir > 0) {
    menuNum++;
    if (menuNum >= MENU_COUNT) menuNum = MENU_COUNT - 1;
  }
  if (dir < 0) {
    menuNum--;
    if (menuNum < 0) menuNum = 0;
  }
  uv[STYLE_SETTING] = styleSettings[(int)uvSTYLE];
}

void swISR() {
  if (millis() - debounceLastTime < debounce_ms) return;
  debounceLastTime = millis();

  swPress = true;
  menuGo = true;
}

void enISR() {
  bool dt = digitalRead(DT);
  bool clk = digitalRead(CLK);
  Serial.println("Interrupt");

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
  menuGo = true;
}

void menu() {
  if (!menuGo) return;

  menuGo = false;
  menuSelectTime = millis();

  float delta = (enIncrement - enDecrement);
  Serial.println(delta);

  if (swPress) selectMenu = !selectMenu;

  if (selectMenu && delta != 0) {
    changeMenu(delta);
  } else {
    if(menuNum == STYLE_SETTING) {
      editUserVariable(delta, &styleSettings[(int)uvSTYLE]);
    } else editUserVariable(delta, &uv[menuNum]);
  }

  applyChanges();

  swPress = false;
  enIncrement = false;
  enDecrement = false;
}
