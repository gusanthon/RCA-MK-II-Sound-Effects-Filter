/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "RCA_MKII_SEF.h"

//==============================================================================
/**
*/
class RCAMKIISoundEffectsFilterAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    RCAMKIISoundEffectsFilterAudioProcessor();
    ~RCAMKIISoundEffectsFilterAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void updateFilters();
    float getCurrentGain();
    
    void computeMagnitudeResponse(std::array<float, fftSize>& result) {dummy.computeMagnitudeResponse(result);}

    std::array<RCA_MK2_SEF, 2>& getFilters() {return filters;}
    
    RCA_MK2_SEF& getDummy() {return dummy;};
        
    bool isHighPassContinuous = true;
    bool isLowPassContinuous = true;
    
    bool highPassControlsChanged = false;
    bool lowPassControlsChanged = false;
        
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

private:
    //==============================================================================
    
    std::array<RCA_MK2_SEF, 2> filters;
    RCA_MK2_SEF dummy;
    
    int prevHighPassKnobPos;
    int prevLowPassKnobPos;
    
    juce::SmoothedValue<float> hpfSmooth[2];
    juce::SmoothedValue<float> lpfSmooth[2];
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RCAMKIISoundEffectsFilterAudioProcessor)
};
