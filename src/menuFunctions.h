void changeStyle()
{
    if (gEncoderState.navigating)
    {
        Serial.printf("\nchangeStyle selected");
    }
    else
    {
        Serial.printf("\nchangeStyle: %d", gEncoderState.delta);
    }
}

void changeLoudness()
{
    if (gEncoderState.navigating)
    {
        Serial.printf("\nchangeLoudness selected");
    }
    else
    {
        Serial.printf("\nchangeLoudness: %d", gEncoderState.delta);
    }
}

void changex()
{
    if (gEncoderState.navigating)
    {
        Serial.printf("\nchangex selected");
    }
    else
    {
        Serial.printf("\nchangex: %d", gEncoderState.delta);
    }
}