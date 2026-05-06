/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SpectralReductionPlugAudioProcessorEditor::SpectralReductionPlugAudioProcessorEditor (SpectralReductionPlugAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), scopeDisplay(p.fft[0]), nub(p.parameters), valueTreeState(p.parameters)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    double ratio = 4.0 / 3.0;
    setResizeLimits(400, 400 / ratio, 1200, 1200 / ratio);
    getConstrainer()->setFixedAspectRatio(ratio);
    setSize(800.0, 800.0 / ratio);

    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::backgroundFinal_png, BinaryData::backgroundFinal_pngSize);

    resolutionAttachment.reset(new SliderAttachment(valueTreeState, "resolution", resolutionSlider));
    addAndMakeVisible(resolutionSlider);
    resolutionSlider.setName("resolutionSlider");
    resolutionSlider.setLabelText("resolution");

    orderAttachment.reset(new SliderAttachment(valueTreeState, "order", orderSlider));
    addAndMakeVisible(orderSlider);
    orderSlider.setName("orderSlider");
    orderSlider.setLabelText("order");

    offsetAttachment.reset(new SliderAttachment(valueTreeState, "offset", offsetSlider));
    addAndMakeVisible(offsetSlider);
    offsetSlider.setName("offsetSlider");
    offsetSlider.setLabelText("offset");

    //addAndMakeVisible(scopeDisplay);
    addAndMakeVisible(nub);

    gainAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    addAndMakeVisible(gainSlider);
    gainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    gainSlider.setName("gainSlider");

}

SpectralReductionPlugAudioProcessorEditor::~SpectralReductionPlugAudioProcessorEditor()
{
}

//==============================================================================
void SpectralReductionPlugAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
    g.drawImage(backgroundImage,
        0, 0, getWidth(), getHeight(),
        0, 0, backgroundImage.getWidth(), backgroundImage.getHeight());
}

void SpectralReductionPlugAudioProcessorEditor::resized()
{
    resolutionSlider.setBounds(getWidth()*xRatio, getHeight()*yRatio1, getWidth()*widthRatio, getHeight()*heightRatio);
    orderSlider.setBounds(getWidth() * xRatio, getHeight() * yRatio2, getWidth() * widthRatio, getHeight() * heightRatio);
    offsetSlider.setBounds(getWidth() * xRatio, getHeight() * yRatio3, getWidth() * widthRatio, getHeight() * heightRatio);
    scopeDisplay.setBounds(getWidth() * xRatio, getHeight() * yRatio4, getWidth() * widthRatio, getHeight() * bottomHRatio);
    nub.setBounds(getWidth() * bottomXRatio, getHeight() * yRatio4, getWidth() * bottomWRatio, getHeight() * bottomHRatio);
    gainSlider.setBounds(getWidth() * knobXRatio, getHeight() * knobYRatio, getWidth() * knobWidthRatio, getHeight() * knobHeightRatio);
}