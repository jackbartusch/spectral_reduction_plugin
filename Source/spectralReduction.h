#pragma once

#include <JuceHeader.h>

class fftProcessor
{
public:
    fftProcessor();

    void FFTReset();
    float processSample(float sample, bool bypassed);
    void processFrame(bool bypassed);
    void processSpectrum(float* data, int numBins);
    void drawSample(float sample, int index);
    void configureFFT(int newOrder);

    int getResolution()
    {
        return resolution;
    }
    int getOrder()
    {
        return fftOrder;
    }
    int getOffset()
    {
        return offset;
    }
    bool needChange()
    {
        return needsReconfigure;
    }
    void setResolution(int newRes)
    {
        resolution = newRes;
    }
    void setOrder(int newOrder)
    {
        fftOrder = newOrder;
    }
    void setOffset(int newOff)
    {
        offset = newOff;
    }

    int fftOrder = 12;
    int fftSize = 1 << fftOrder;
    int numBins = fftSize / 2 + 1;
    int overlap = 4;
    int hopSize = fftSize / overlap;

    int scopeSize = fftSize / 2;
    std::vector<float> scopeData;

    bool scopeFlag = false;
    bool needsReconfigure = false;

private:
    static constexpr float windowCorrection = 2.0f / 3.0f;

    // Counts up until the next hop.
    int count = 0;
    // Write position in input FIFO and read position in output FIFO.
    int pos = 0;

    juce::dsp::FFT forwardFFT; // [4]
    std::unique_ptr<juce::dsp::WindowingFunction<float>> window; // [5]

    std::vector<float> inputFifo;
    std::vector<float> outputFifo;
    std::vector<float> fftData;
    std::vector<float> magnitudeArr;

    int resolution = 1;
    int offset = 0;

};