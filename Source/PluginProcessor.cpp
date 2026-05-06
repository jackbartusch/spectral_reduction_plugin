/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "lookAndFeel.h"

//==============================================================================
SpectralReductionPlugAudioProcessor::SpectralReductionPlugAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
    ,
    parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    resParam = parameters.getRawParameterValue("resolution");
    ordParam = parameters.getRawParameterValue("order");
    offParam = parameters.getRawParameterValue("offset");
    gainParam = parameters.getRawParameterValue("gain");
}

SpectralReductionPlugAudioProcessor::~SpectralReductionPlugAudioProcessor()
{
}

//==============================================================================
const juce::String SpectralReductionPlugAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SpectralReductionPlugAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SpectralReductionPlugAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SpectralReductionPlugAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SpectralReductionPlugAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SpectralReductionPlugAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SpectralReductionPlugAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SpectralReductionPlugAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SpectralReductionPlugAudioProcessor::getProgramName (int index)
{
    return {};
}

void SpectralReductionPlugAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SpectralReductionPlugAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    fft[0].FFTReset();
    fft[1].FFTReset();

    int curRes = *resParam;
    int curOrd = *ordParam;
    int curOff = *offParam;

    prevGain = *gainParam;
}

void SpectralReductionPlugAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SpectralReductionPlugAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SpectralReductionPlugAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto* readPtrL = buffer.getReadPointer(0);
    auto* readPtrR = buffer.getReadPointer(1);
    auto* channelL = buffer.getWritePointer(0);
    auto* channelR = buffer.getWritePointer(1);
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        float sampleL = readPtrL[i];
        float sampleR = readPtrR[i];

        sampleL = fft[0].processSample(sampleL, false);
        sampleR = fft[1].processSample(sampleR, false);

        channelL[i] = sampleL;
        channelR[i] = sampleR;
    }

    int curRes = *resParam;
    int curOrd = *ordParam;
    int curOff = *offParam;

    //apply gain
    float currGain = *gainParam;
    if (juce::approximatelyEqual(currGain, prevGain))
    {
        buffer.applyGain(currGain);
    }
    else
    {
        buffer.applyGainRamp(0, buffer.getNumSamples(), prevGain, currGain);
        prevGain = currGain;
    }

    if (curRes != fft[0].getResolution())
    {
        fft[0].setResolution(curRes);
        fft[1].setResolution(curRes);
    }
    
    if (curOff != fft[0].getOffset())
    {
        fft[0].setOffset(curOff);
        fft[1].setOffset(curOff);
    }
    if (curOrd != fft[0].getOrder())
    {
        fft[0].configureFFT(curOrd);
        fft[1].configureFFT(curOrd);
    }
    
}

//==============================================================================
bool SpectralReductionPlugAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SpectralReductionPlugAudioProcessor::createEditor()
{
    return new SpectralReductionPlugAudioProcessorEditor (*this);
}

//==============================================================================
void SpectralReductionPlugAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void SpectralReductionPlugAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SpectralReductionPlugAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout SpectralReductionPlugAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterInt>("resolution", "Resolution", 1, 24, 1));
    params.push_back(std::make_unique<juce::AudioParameterInt>("order", "Order", 2, 12, 10));
    params.push_back(std::make_unique<juce::AudioParameterInt>("offset", "Offset", 0, 11, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.0f, 1.0f, 0.5f));

    return { params.begin(), params.end() };
}

