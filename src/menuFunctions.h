CRGB editCol = CRGB::Aqua;
CRGB viewCol = CRGB::White;
CRGB peekCol = CRGB::Orange;

void changeStyle()
{
    static double num = 0;
    num += 1.1 * gEncoderState.delta;
    char title[6];
    char value[6];
    int newmode;

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
        newmode = matrix.scrollMode(gEncoderState.delta);
        matrix.setMenuColour(editCol);
        matrix.getModeString(value);
        matrix.drawString(value, 0, BOTTOM_LINE);

        switch (newmode)
        {
        case SPECTROGRAM:
            processor.setGain(1);
            break;
        case COLOUR_BC:
            processor.setGain(2);
            break;
        case COLOUR_BCW:
            processor.setGain(10);
            break;
        case RAINBOW_BC:
            processor.setGain(2);
            break;
        case RAINBOW_BCW:
            processor.setGain(10);
            break;
        }
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
        menu.skip(gEncoderState.delta);
        break;

    default:
        break;
    }
}

void changeGain()
{
    char title[6];
    char val[6];
    strcpy(title, "RAMP");

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
    int min = 3500;
    int max = 44100;
    double sr = (double)mic.getSampleRate();
    int step = 100;
    sr += dir * step;

    if (sr > max)
        sr = max;
    if (sr < min)
        sr = min;

    mic.setSampleRate((uint32_t)sr);
    processor.setSampleRate((int)sr);
    fft.setSampleRate((int)sr);

    return sr / 1000;
}

void setSampleRate(int sr)
{
    mic.setSampleRate((uint32_t)sr);
    fft.setSampleRate((int)sr);
}

void changeSampleRate()
{
    char title[6];
    char val[6];
    strcpy(title, "SRAT");

    if (!gEncoderState.navigating)
        gEncoderState.navigating = true;

    matrix.clearMenu();
    matrix.setMenuColour(editCol);
    matrix.drawString(title, 0, TOP_LINE);
    matrix.setMenuColour(peekCol);
    float sr = (float)processor.getSampleRate() / 1000;
    sprintf(val, "%.1f", sr);
    matrix.drawString(val, 0, BOTTOM_LINE);
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

        matrix.setMenuColour(peekCol);
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

        matrix.setMenuColour(CRGB::Orange);
        sprintf(val2, "%d", fpsActual);
        matrix.drawString(val2, 9, BOTTOM_LINE);
    }
}

void changeLoVol()
{

    char title[6] = "vol";
    char title2[6] = "lo";
    char val[6];

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.drawString(title2, 0, TOP_LINE + 5);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%d", processor.incrementLoThreshold(0));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.drawString(title2, 0, TOP_LINE + 5);
        matrix.setMenuColour(editCol);
        sprintf(val, "%d", processor.incrementLoThreshold(gEncoderState.delta));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

void changeHiVol()
{

    char title[6] = "vol";
    char title2[6] = "hi";
    char val[6];

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.drawString(title2, 0, TOP_LINE + 5);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%d", processor.incrementHiThreshold(0));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.drawString(title2, 0, TOP_LINE + 5);
        matrix.setMenuColour(editCol);
        sprintf(val, "%d", processor.incrementHiThreshold(gEncoderState.delta));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

void changeVolPeak()
{

    char title[6] = "vol";
    char title2[6] = "peak";
    char val[6];

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.drawString(title2, 0, TOP_LINE + 5);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%.2f", processor.getVolActual());
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.drawString(title2, 0, TOP_LINE + 5);
        matrix.setMenuColour(editCol);
        sprintf(val, "%.2f", processor.incrementVolPeak(gEncoderState.delta));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

void changeLoNote()
{
    char title[6] = "note";
    char title2[6] = "lo";
    char val[6];

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.drawString(title2, 0, TOP_LINE + 5);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%d", processor.incrementLoNote(0));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.drawString(title2, 0, TOP_LINE + 5);
        matrix.setMenuColour(editCol);
        sprintf(val, "%d", processor.incrementLoNote(gEncoderState.delta));
        setSampleRate(processor.getSampleRate());
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

void changeNPB()
{
    char title[6] = "npb";
    char val[6];

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%d", processor.incrementNPB(0));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        sprintf(val, "%d", processor.incrementNPB(gEncoderState.delta));
        setSampleRate(processor.getSampleRate());
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

void changeOSR()
{
    char title[6] = "osr";
    char val[6];

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%.2f", processor.incrementOSR(0));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        sprintf(val, "%.2f", processor.incrementOSR(gEncoderState.delta));
        setSampleRate(processor.getSampleRate());
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}