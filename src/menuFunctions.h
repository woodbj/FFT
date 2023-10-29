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

void applyNewSampleRate()
{
    
}

void changeSampleRate()
{
    char title1[6];
    char title2[6];
    strcpy(title1, "SPL");
    strcpy(title2, "RATE");

    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.setMenuColour(editCol);
        matrix.drawString(title1, 0, TOP_LINE);
        matrix.drawString(title2, 0, TOP_LINE + 5);
        matrix.setMenuColour(viewCol);
    }
    else
    {
        matrix.clearMenu();
        matrix.setMenuColour(viewCol);
        matrix.drawString(title1, 0, TOP_LINE);
        matrix.drawString(title2, 0, TOP_LINE + 5);
        matrix.setMenuColour(editCol);
    }
}