CRGB editCol = CRGB::Aqua;
CRGB viewCol = CRGB::White;

void changeStyle()
{
    static double num = 0;
    num += 1.1 * gEncoderState.delta;
    char title[6];
    char value[6];

    strcpy(title, "STYL");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        matrix.getModeString(value);
        matrix.drawString(value, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.scrollMode(gEncoderState.delta);
        matrix.setMenuColour(editCol);
        matrix.getModeString(value);
        matrix.drawString(value, 0, BOTTOM_LINE);
    }
}

void spectrogramMode()
{
    char title[6];
    strcpy(title, "PALT");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
    }
}

void colourMode()
{
    char title[6];
    char val[6];
    strcpy(title, "Col");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%d", matrix.incrementColour(0));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);        
        sprintf(val, "%d", matrix.incrementColour(gEncoderState.delta));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

void rainbowMode()
{
    char title[6];
    char val[6];
    strcpy(title, "RATE");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%d", matrix.incrementRainbowRate(0));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        sprintf(val, "%d", matrix.incrementRainbowRate(gEncoderState.delta));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

void changeStyleParameter()
{
    switch (matrix.getCurrentMode())
    {
    case RAINBOW_BC:
        rainbowMode();
        break;

    case RAINBOW_BCW:
        rainbowMode();
        break;

    case COLOUR_BC:
        colourMode();
        break;

    case COLOUR_BCW:
        colourMode();
        break;

    case SPECTROGRAM:
        spectrogramMode();
        break;

    default:
        break;
    }
}

void changeGain()
{
    char title[6];
    char val[6];
    strcpy(title, "GAIN");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%.2f", processor.incrementGain(0));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        sprintf(val, "%.2f", processor.incrementGain(gEncoderState.delta));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

double adjustSampleRate(int dir)
{
    int min = 10000;
    int max = 44100;
    double sr = (int)mic.getSampleRate();
    int step = 500;
    sr += dir * step;

    if (sr > max)
        sr = max;
    if (sr < min)
        sr = min;

    mic.setSampleRate((uint32_t)sr);
    processor.setSampleRate((int)sr);
    fft.setSampleRate((int)sr);

    Serial.printf("\nSample Rate %fHz", sr);

    return sr / 1000;
}

void changeSampleRate()
{
    char title[6];
    char val[6];
    strcpy(title, "SRAT");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%.1f", adjustSampleRate(0));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        sprintf(val, "%.1f", adjustSampleRate(gEncoderState.delta));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

void changeFPS()
{
    char title[6];
    char val1[6];
    char val2[6];
    strcpy(title, "FPS");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);

        matrix.setMenuColour(viewCol);
        sprintf(val1, "%d", fpsRequested);
        matrix.drawString(val1, 0, BOTTOM_LINE);

        matrix.setMenuColour(viewCol);
        sprintf(val2, "%d", fpsActual);
        matrix.drawString(val2, 9, BOTTOM_LINE);
    }
    else
    {
        fpsRequested += gEncoderState.delta;
        fpsRequested = constrain(fpsRequested, 20, 40);

        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);

        matrix.setMenuColour(editCol);
        sprintf(val1, "%d", fpsRequested);
        matrix.drawString(val1, 0, BOTTOM_LINE);

        matrix.setMenuColour(viewCol);
        sprintf(val2, "%d", fpsActual);
        matrix.drawString(val2, 9, BOTTOM_LINE);
    }
}