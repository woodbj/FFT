#include <ComputeFFT.h>

ComputeFFT::ComputeFFT(ComputeFFT_Parameters_t _parameters)
{
    parameters = _parameters;
    FFT = arduinoFFT(
        parameters.real,
        parameters.imag,
        parameters.sampleCount,
        parameters.sampleRate);

    setWindow(parameters.window);

    
}

void ComputeFFT::go(short *input)
{
    prepareInput(input);
    applyWindow();
    FFT.Compute(FFT_FORWARD);
    getMagnitude();
}

void ComputeFFT::applyWindow()
{
    for (int i = 0; i < parameters.sampleCount; i++){
        parameters.real[i] *= parameters.windowArray[i];
    }
}

void ComputeFFT::prepareInput(short * input)
{
    for (int i = 0; i < parameters.sampleCount; i++){
        parameters.real[i] = input[i];
        parameters.imag[i] = 0;
    }
    
}

void ComputeFFT::getMagnitude()
{
    double invsamples = 1.0f / parameters.sampleCount;
    double pythag;
    for (int i = 1; i < parameters.sampleCount / 2; i++){
        pythag = parameters.real[i] * parameters.real[i] + parameters.imag[i] * parameters.imag[i];
        parameters.real[i] = sqrt(pythag) * invsamples;
    }
}

void ComputeFFT::setWindow(enum Windows window)
{
    currentWindow = window;
    if (currentWindow == RECTANGULAR)
        return;
    for (int i = 0; i < parameters.sampleCount; i++)
    {
        float z = TWOPI * i / (parameters.sampleCount * 1.0f);
        switch (window)
        {  
        case HANNING:
            parameters.windowArray[i] = (1 - cos(z)) / 2.0f;
            break;

        case HAMMING:
            parameters.windowArray[i] = (0.54 - 0.46 * cos(z));
            break;

        case TRIANGULAR:
            parameters.windowArray[i] = 1 - abs((i - parameters.sampleCount / 2.0f) / (parameters.sampleCount / 2.0f));
            break;

        case SINE:
            parameters.windowArray[i] = sin(z / 2.0f);
            break;
        }
    }
}
