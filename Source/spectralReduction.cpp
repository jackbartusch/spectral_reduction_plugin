#include "spectralReduction.h"

fftProcessor::fftProcessor() :
    forwardFFT(10)
{
    configureFFT(10);
}

void fftProcessor::FFTReset()
{
    count = 0;
    pos = 0;

    std::fill(inputFifo.begin(), inputFifo.end(), 0.0f);
    std::fill(outputFifo.begin(), outputFifo.end(), 0.0f);

}

float fftProcessor::processSample(float sample, bool bypassed)
{
    inputFifo[pos] = sample;
    float outputSample = outputFifo[pos];
    outputFifo[pos] = 0.0f;
    
    pos++;
    if (pos == fftSize)
    {
        pos = 0;
    }

    count++;
    if (count == hopSize)
    {
        count = 0;
        processFrame(bypassed);
    }

    return outputSample;

}

void fftProcessor::processFrame(bool bypassed)
{
    const float* inputPtr = inputFifo.data();
    float* fftPtr = fftData.data();

    std::memcpy(fftPtr, inputPtr + pos, (fftSize - pos) * sizeof(float));
    if (pos > 0) {
        std::memcpy(fftPtr + fftSize - pos, inputPtr, pos * sizeof(float));
    }

    window->multiplyWithWindowingTable(fftPtr, fftSize);

    if (!bypassed) {
        // Perform the forward FFT.
        forwardFFT.performRealOnlyForwardTransform(fftPtr, true);

        // Do stuff with the FFT data.
        processSpectrum(fftPtr, numBins);

        // Perform the inverse FFT.
        forwardFFT.performRealOnlyInverseTransform(fftPtr);
    }

    // Apply the window again for resynthesis.
    window->multiplyWithWindowingTable(fftPtr, fftSize);

    for (int i = 0; i < fftSize; ++i) {
        fftPtr[i] *= windowCorrection;
    }

    // Add the IFFT results to the output FIFO.
    for (int i = 0; i < pos; ++i) {
        outputFifo[i] += fftData[i + fftSize - pos];
    }
    for (int i = 0; i < fftSize - pos; ++i) {
        outputFifo[i + pos] += fftData[i];
    }
}

void fftProcessor::processSpectrum(float* data, int numBins)
{
    // The spectrum data is floats organized as [re, im, re, im, ...]
    // but it's easier to deal with this as std::complex values.
    auto* cdata = reinterpret_cast<std::complex<float>*>(data);

    for (int i = 0; i < numBins; ++i)
    {
        int bin = (offset + i) % numBins;

        float magnitude = std::abs(cdata[bin]);
        float phase = std::arg(cdata[bin]);

        if (bin % (resolution) != 0 && bin > 0)
        {
            magnitude = std::abs(cdata[bin - 1]);
        }

        drawSample(magnitude, bin);

        // Convert magnitude and phase back into a complex number.
        cdata[bin] = std::polar(magnitude, phase);
    }
} //if you reach the end and haven't filled all the samples, wrap around, 

void fftProcessor::drawSample(float sample, int index)
{
    auto mindB = -100.0f;
    auto maxdB = 0.0f;
    magnitudeArr[index] = sample;
    if (index == numBins - 1)
    {
        for (int i = 0; i < numBins; ++i)
        {
            auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)numBins) * 0.2f);
            auto fftDataIndex = juce::jlimit(0, fftSize / 2, (int)(skewedProportionX * (float)fftSize * 0.5f));
            auto level = juce::jmap(juce::jlimit(mindB, maxdB, juce::Decibels::gainToDecibels(magnitudeArr[fftDataIndex]) - juce::Decibels::gainToDecibels((float)fftSize)),
                mindB,
                maxdB,
                0.0f,
                1.0f);
            scopeData[i] = level; // [4]
        }
        scopeFlag = true;
    }
}

void fftProcessor::configureFFT(int newOrder)
{
    fftOrder = newOrder;
    fftSize = 1 << fftOrder;
    numBins = fftSize / 2 + 1;
    hopSize = fftSize / overlap;
    scopeSize = fftSize / 2;

    forwardFFT = juce::dsp::FFT(fftOrder);
    window = std::make_unique<juce::dsp::WindowingFunction<float>>(fftSize, juce::dsp::WindowingFunction<float>::hann);
    inputFifo.resize(fftSize);
    outputFifo.resize(fftSize);
    fftData.resize(2 * fftSize);
    magnitudeArr.resize(numBins);
    scopeData.clear();
    scopeData.resize(numBins);

    pos = 0;
    count = 0;
}