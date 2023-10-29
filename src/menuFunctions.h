CRGB editCol = CRGB::Red;
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
    strcpy(title, "Col");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        matrix.drawDecimal(matrix.incrementColour(0), 0, BOTTOM_LINE, 0);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        matrix.drawDecimal(matrix.incrementColour(gEncoderState.delta), 0, BOTTOM_LINE, 0);
    }
}

void rainbowMode()
{
    char title[6];
    strcpy(title, "RATE");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        matrix.drawDecimal(matrix.incrementRainbowRate(0), 0, BOTTOM_LINE, 0);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        matrix.drawDecimal(matrix.incrementRainbowRate(gEncoderState.delta), 0, BOTTOM_LINE, 0);
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
    strcpy(title, "GAIN");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        matrix.drawDecimal(processor.incrementGain(0), 0, BOTTOM_LINE, 3);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        matrix.drawDecimal(processor.incrementGain(gEncoderState.delta), 0, BOTTOM_LINE, 3);
    }
}

double adjustSampleRate(int dir)
{
    int min = 10000;
    int max = 44100;
    double sr = (int)mic.getSampleRate();
    int step = 500;
    sr += dir * step;

    if(sr > max) sr = max;
    if(sr < min) sr = min;

    mic.setSampleRate((uint32_t)sr);
    processor.setSampleRate((int)sr);
    fft.setSampleRate((int)sr);

    Serial.printf("\nSample Rate %fHz", sr);

    return sr / 1000;
}

void changeSampleRate()
{
    char title[6];
    strcpy(title, "SRAT");


    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(viewCol);
        matrix.drawDecimal(adjustSampleRate(0), 0, BOTTOM_LINE, 3);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title, 0, TOP_LINE);
        matrix.setMenuColour(editCol);
        matrix.drawDecimal(0, 0, BOTTOM_LINE, 0);
        matrix.drawDecimal(adjustSampleRate(gEncoderState.delta), 0, BOTTOM_LINE, 3);
    }

}