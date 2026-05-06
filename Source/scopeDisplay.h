#pragma once

#include <JuceHeader.h>
#include "spectralReduction.h"

class ScopeComponent : public juce::Component,
    public juce::Timer
{
public:
    ScopeComponent(fftProcessor& processorRef) : processor(processorRef) { startTimerHz(10); }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour());

        juce::Rectangle<int> scopeArea = getLocalBounds();
        const int width = scopeArea.getWidth();
        const int height = scopeArea.getHeight();
        const int x = scopeArea.getX();
        const int y = scopeArea.getY();

        g.setColour(juce::Colour(137, 139, 255));

        for (int i = 1; i < processor.scopeSize; ++i)
        {
            float x1 = juce::jmap((float)(i - 1), 0.0f, (float)(processor.scopeSize - 1), (float)x, (float)(x + width));
            float y1 = juce::jmap(processor.scopeData[i - 1], 0.0f, 1.0f, (float)(y + height), (float)y);

            float x2 = juce::jmap((float)i, 0.0f, (float)(processor.scopeSize - 1), (float)x, (float)(x + width));
            float y2 = juce::jmap(processor.scopeData[i], 0.0f, 1.0f, (float)(y + height), (float)y);

            g.drawLine(x1, y1, x2, y2, getWidth() * 0.005);
        }
    }
    void timerCallback() override { repaint(); }

private:
    fftProcessor& processor;
};
