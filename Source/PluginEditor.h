/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "scopeDisplay.h"
#include "lookAndFeel.h"
#include "nubDrawing.h"

//==============================================================================
/**
*/
class SpectralReductionPlugAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    SpectralReductionPlugAudioProcessorEditor (SpectralReductionPlugAudioProcessor&);
    ~SpectralReductionPlugAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SpectralReductionPlugAudioProcessor& audioProcessor;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;

    CustomSlider resolutionSlider;
    CustomSlider orderSlider;
    CustomSlider offsetSlider;
    CustomRotary gainSlider;

    nubComp nub;

    juce::AudioProcessorValueTreeState& valueTreeState;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resolutionAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> orderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> offsetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    ScopeComponent scopeDisplay;

    CustomLookAndFeel customLookAndFeel;
    juce::Image backgroundImage;

    const float backWidth = 4000.0f, boxWidth = 3640, boxX = 180, bottomX = 560, bottomWidth = 2880;
    const float backHeight = 3000.0f, bottomHeight = 660, boxHeight = 540, boxY1 = 120, boxY2 = 720, boxY3 = 1320, boxY4 = 2070;

    const float xRatio = boxX/backWidth, widthRatio = boxWidth/backWidth, bottomXRatio = bottomX/backWidth, bottomWRatio = bottomWidth/backWidth;
    const float yRatio1 = boxY1/backHeight, yRatio2 = boxY2/backHeight, yRatio3 = boxY3/backHeight, yRatio4 = boxY4/backHeight;
    const float heightRatio = boxHeight/backHeight, bottomHRatio = bottomHeight/backHeight;

    const float knobX = 310, knobY = 2070, knobSize = 120;
    const float knobXRatio = knobX / backWidth, knobYRatio = knobY / backHeight, knobWidthRatio = knobSize / backWidth, knobHeightRatio = knobSize / backHeight;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectralReductionPlugAudioProcessorEditor)
};
