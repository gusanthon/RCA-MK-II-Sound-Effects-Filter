/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

#include "ParameterPanel.h"
#include "ResponseCurveComponent.h"
#include "TopBarComponent.h"
#include "CustomToggle.h"

//==============================================================================
/**
*/
class RCAMKIISoundEffectsFilterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RCAMKIISoundEffectsFilterAudioProcessorEditor (RCAMKIISoundEffectsFilterAudioProcessor&);
    ~RCAMKIISoundEffectsFilterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    RCAMKIISoundEffectsFilterAudioProcessor& audioProcessor;
    
    TopBarComponent topBar;
    const int topBarHeight = 35;

    using apvts = juce::AudioProcessorValueTreeState;

    /** High pass panel */
    SliderWithLabel HighPassSlider {"CUTOFF FREQ"};
    SliderWithLabel DiscHighPassSlider {"CUTOFF POS"};
    std::unique_ptr<apvts::SliderAttachment> highPassSliderAttachment;
    std::unique_ptr<apvts::SliderAttachment> discreteHighPassAttachment;
    
    CustomToggle highPassModToggle {"MOD"};
    ControlsToggle highPassControls {"CONTROLS"};
    FilterPanel highPassParams {juce::Array<juce::Component*>{&highPassControls, &highPassModToggle}, HighPassSlider, DiscHighPassSlider};
    
    /** Low pass panel */
    SliderWithLabel LowPassSlider {"CUTOFF FREQ"};
    SliderWithLabel DiscLowPassSlider {"CUTOFF POS"};
    std::unique_ptr<apvts::SliderAttachment> lowPassSliderAttachment;
    std::unique_ptr<apvts::SliderAttachment> discreteLowPassAttachment;

    CustomToggle lowPassModToggle {"MOD"};
    ControlsToggle lowPassControls {"CONTROLS"};
    FilterPanel lowPassParams {juce::Array<juce::Component*>{&lowPassControls, &lowPassModToggle}, LowPassSlider, DiscLowPassSlider};

    
    /** Master panel */
    SliderWithLabel ZInputSlider {"Z INPUT"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ZInputAttachment;
    
    SliderWithLabel ZOutputSlider {"Z OUTPUT"};
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ZOutputAttachment;
    
    SliderWithLabel OutputGainSlider {"OUTPUT GAIN"};
    std::unique_ptr<apvts::SliderAttachment> outputGainSliderAttachment;

    ParameterPanel MasterParams {juce::Array<juce::Component*>{&ZInputSlider, &ZOutputSlider, &OutputGainSlider}};
    
    /** Main container */
    ParameterPanel Container {juce::Array<juce::Component*>{&highPassParams, &MasterParams, &lowPassParams}};
    
    /** Response curve */
    ResponseCurveComponent responseCurve;
    juce::ToggleButton responseCurveToggle;
    const float curveToParamsRatio = 1.5f;


    void initialiseHighPassParams(RCAMKIISoundEffectsFilterAudioProcessor& p);
    void initialiseLowPassParams(RCAMKIISoundEffectsFilterAudioProcessor& p);
    void initialiseMasterParams(RCAMKIISoundEffectsFilterAudioProcessor& p);
    void initialiseTopBar(RCAMKIISoundEffectsFilterAudioProcessor& p);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RCAMKIISoundEffectsFilterAudioProcessorEditor)
};
