void changeStyle()
{

    char title[5];
    strcpy(title, "STYL");
    if (gEncoderState.navigating)
    {
        matrix.clearMenu();
        matrix.drawString(title, 0, 0);
    }
    else
    {
        int dir = constrain(gEncoderState.delta, -1, 1);
        matrix.drawString(title, 0, 11);
        matrix.setMode(dir);
    }
}