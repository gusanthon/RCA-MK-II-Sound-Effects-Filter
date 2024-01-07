/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RCAMKIISoundEffectsFilterAudioProcessor::RCAMKIISoundEffectsFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

juce::AudioProcessorValueTreeState::ParameterLayout RCAMKIISoundEffectsFilterAudioProcessor::createParameters()
{
    AudioProcessorValueTreeState::ParameterLayout params;
    
    juce::NormalisableRange<float> highPassRange(20, 20000);
    highPassRange.setSkewForCentre(800);

    juce::NormalisableRange<float> lowPassRange(20, 20000);
    lowPassRange.setSkewForCentre(2000);
    
    params.add(std::make_unique<juce::AudioParameterInt>(ParameterID{"DISC_HIGH_PASS", 1}, "High Pass Cutoff", 1, 11, 1));
    
    params.add(std::make_unique<juce::AudioParameterInt>(ParameterID{"DISC_LOW_PASS", 1}, "Low Pass Cutoff", 1, 11, 11));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"HIGH_PASS_CUTOFF", 1}, "High Pass Cutoff", highPassRange, 20));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"LOW_PASS_CUTOFF", 1}, "Low Pass Cutoff", lowPassRange, 20000));

    params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"Z_INPUT", 1}, "Z input", -100.f, 100.f, 0.f));

    params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"Z_OUTPUT", 1}, "Z output", -100.f, 100.f, 0.f));
    
    params.add(std::make_unique<juce::AudioParameterFloat>(ParameterID{"OUTPUT_GAIN", 1}, "Output gain", 0., 20., 6.));

    return params;
}

RCAMKIISoundEffectsFilterAudioProcessor::~RCAMKIISoundEffectsFilterAudioProcessor()
{
}

//==============================================================================
const juce::String RCAMKIISoundEffectsFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RCAMKIISoundEffectsFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RCAMKIISoundEffectsFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RCAMKIISoundEffectsFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RCAMKIISoundEffectsFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RCAMKIISoundEffectsFilterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RCAMKIISoundEffectsFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RCAMKIISoundEffectsFilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RCAMKIISoundEffectsFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void RCAMKIISoundEffectsFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void RCAMKIISoundEffectsFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void RCAMKIISoundEffectsFilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RCAMKIISoundEffectsFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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


float RCAMKIISoundEffectsFilterAudioProcessor::getCurrentGain()
{
    const float gDb = apvts.getRawParameterValue("OUTPUT_GAIN")->load();
    return juce::Decibels::decibelsToGain(gDb);
}

void RCAMKIISoundEffectsFilterAudioProcessor::updateFilters()
{
    
    const float lpfValue = apvts.getRawParameterValue("LOW_PASS_CUTOFF")->load();
    const float hpfValue = apvts.getRawParameterValue("HIGH_PASS_CUTOFF")->load();
    const int lpfKnobPos = apvts.getRawParameterValue("DISC_LOW_PASS")->load();
    const int hpfKnobPos = apvts.getRawParameterValue("DISC_HIGH_PASS")->load();
    
    auto setLowPassParameters = [this](RCA_MK2_SEF& filter, float cutoff, int knobPos)
    {
        if (isLowPassContinuous)
            filter.setLowPassCutoff(cutoff);
        else
            filter.setLowPassKnobPos(knobPos);
    };
    
    auto setHighPassParameters = [this](RCA_MK2_SEF& filter, float cutoff, int knobPos)
    {
        if (isHighPassContinuous)
            filter.setHighPassCutoff(cutoff);
        else
            filter.setHighPassKnobPos(knobPos);
    };
    
    for (auto& filter : filters)
    {
        setLowPassParameters(filter, lpfValue, lpfKnobPos);
        setHighPassParameters(filter, hpfValue, hpfKnobPos);
    }
    
    setLowPassParameters(dummy, lpfValue, lpfKnobPos);
    setHighPassParameters(dummy, hpfValue, hpfKnobPos);
    
}

template <typename T>
bool IsInBounds(const T& value, const T& low, const T& high)
{
    return (value >= low) && (value <= high);
}


float mapImpedanceVal(float value)
{
    if (value == 0) return 560.f;
    if (IsInBounds(value, -100.f, 0.f))
        return juce::jmap(value, -100.f, 0.f, .001f, 560.f);
    if (IsInBounds(value, 0.f, 100.f))
        return juce::jmap(value, 0.f, 100.f, 560.f, 5000.f);
    jassertfalse;
};


void RCAMKIISoundEffectsFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    float lowPassCutoff = apvts.getRawParameterValue("LOW_PASS_CUTOFF")->load();
    float highPassCutoff = apvts.getRawParameterValue("HIGH_PASS_CUTOFF")->load();
    
    int highPassKnobPos = apvts.getRawParameterValue("DISC_HIGH_PASS")->load();
    int lowPassKnobPos = apvts.getRawParameterValue("DISC_LOW_PASS")->load();
    

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
        hpfSmooth[i].setTargetValue(highPassCutoff);
        lpfSmooth[i].setTargetValue(lowPassCutoff);

    }
        


    if (highPassKnobPos != prevHighPassKnobPos || lowPassKnobPos != prevLowPassKnobPos)
    {
        for (auto& filter : filters)
            filter.reset();
        prevHighPassKnobPos = highPassKnobPos;
        prevLowPassKnobPos = lowPassKnobPos;
        
    }

    float ZInput = apvts.getRawParameterValue("Z_INPUT")->load();
    float ZOutput = apvts.getRawParameterValue("Z_OUTPUT")->load();
    
    float gainDB = apvts.getRawParameterValue("OUTPUT_GAIN")->load();
    float gain = juce::Decibels::decibelsToGain(gainDB);
    
    float mappedZIn = mapImpedanceVal(ZInput);
    float mappedZOut = mapImpedanceVal(ZOutput);
    
    dummy.setLowPassCutoff(lowPassCutoff);
    dummy.setHighPassCutoff(highPassCutoff);
    dummy.setInputImpedance(mappedZIn);
    dummy.setOutputImpedance(mappedZOut);
    
    updateFilters();
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        hpfSmooth[channel].setTargetValue(highPassCutoff);
        lpfSmooth[channel].setTargetValue(lowPassCutoff);
        
        filters[channel].setInputImpedance(mappedZIn);
        filters[channel].setOutputImpedance(mappedZOut);
            

        auto* channelData = buffer.getWritePointer (channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            const float inSample = channelData[sample];
            
            const float outSample = filters[channel].processSample(inSample);
            
            channelData[sample] = gain * outSample;
        }
    }
}

//==============================================================================
bool RCAMKIISoundEffectsFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* RCAMKIISoundEffectsFilterAudioProcessor::createEditor()
{
    return new RCAMKIISoundEffectsFilterAudioProcessorEditor (*this);
}

//==============================================================================
void RCAMKIISoundEffectsFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RCAMKIISoundEffectsFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RCAMKIISoundEffectsFilterAudioProcessor();
}
