CRGB editCol = CRGB::Aqua;
CRGB viewCol = CRGB::White;
CRGB peekCol = CRGB::Orange;

// sub functions

void setSampleRate()
{
    Serial.printf("\n applying new sr to mic and fft");

    int sr = processor.getSampleRate();

    mic.setSampleRate((uint32_t)sr);
    fft.setSampleRate((int)sr);

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
        // matrix.setMenuColour(viewCol);
        // matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        // matrix.incrementColour(gEncoderState.delta);
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
        // matrix.setMenuColour(viewCol);
        // matrix.drawString(title, 0, TOP_LINE);
        // matrix.setMenuColour(editCol);
        // matrix.incrementRainbowRate(gEncoderState.delta);
        sprintf(val, "%d", matrix.incrementRainbowRate(gEncoderState.delta));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

// menu functions

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
            processor.setGain(5);
            break;
        case RAINBOW_BC:
            processor.setGain(2);
            break;
        case RAINBOW_BCW:
            processor.setGain(5);
            break;
        }
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

void changeVolTarget()
{

    char title[6] = "vol";
    char title2[6] = "hi";
    char val[6];

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        sprintf(val, "%.2f", processor.incrementVolTarget(0));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
    else
    {
        matrix.clearMenu();
        // matrix.setMenuColour(viewCol);
        // matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        sprintf(val, "%.2f", processor.incrementVolTarget(gEncoderState.delta));
        matrix.drawString(val, 0, BOTTOM_LINE);
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
        // matrix.setMenuColour(viewCol);
        // matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        // processor.incrementGain(gEncoderState.delta);
        sprintf(val, "%.2f", processor.incrementGain(gEncoderState.delta));
        matrix.drawString(val, 0, BOTTOM_LINE);
    }
}

void changeFirstNote()
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
        // matrix.setMenuColour(viewCol);
        // matrix.drawString(title, 0, TOP_LINE);
        // matrix.drawString(title2, 0, TOP_LINE + 5);
        // matrix.setMenuColour(editCol);
        // processor.incrementLoNote(gEncoderState.delta);
        sprintf(val, "%d", processor.incrementLoNote(gEncoderState.delta));
        setSampleRate();
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
        // matrix.setMenuColour(viewCol);
        // matrix.drawString(title, 0, TOP_LINE);
        // matrix.setMenuColour(editCol);
        // processor.incrementNPB(gEncoderState.delta);
        sprintf(val, "%d", processor.incrementNPB(gEncoderState.delta));
        setSampleRate();
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
        sprintf(val2, "%d", fpsRequested);
        matrix.drawString(val2, 9, BOTTOM_LINE);
    }
    else
    {
        fpsRequested += gEncoderState.delta;
        fpsRequested = constrain(fpsRequested, 20, 44);

        matrix.clearMenu();
        
        matrix.setMenuColour(editCol);
        sprintf(val1, "%d", fpsRequested);
        matrix.drawString(val1, 0, BOTTOM_LINE);
    }
}
