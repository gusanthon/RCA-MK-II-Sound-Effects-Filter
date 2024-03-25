/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RCAMKIISoundEffectsFilterAudioProcessorEditor::RCAMKIISoundEffectsFilterAudioProcessorEditor (RCAMKIISoundEffectsFilterAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), responseCurve(p)
{
    initialiseHighPassParams(p);
    initialiseLowPassParams(p);
    initialiseMasterParams(p);
    initialiseTopBar(p);
    
    Container.setBackgroundColor(juce::Colour::fromRGB(15, 15, 15));
    addAndMakeVisible(Container);
    addAndMakeVisible(topBar);
    
    setSize (950, 500);
    setResizable(true, true);

}

void RCAMKIISoundEffectsFilterAudioProcessorEditor::initialiseTopBar(RCAMKIISoundEffectsFilterAudioProcessor& p)
{
    topBar.responseCurveToggle.onClick = [&]()
    {
        bool state = topBar.responseCurveToggle.getToggleState();
        
        responseCurve.responseCurveChanged(state);
        responseCurve.setVisible(state);
        
        if (state)
        {
            setSize(getWidth(), getHeight() * 1.5 - topBarHeight);
            responseCurve.hide(false);
        }
        else
        {
            setSize(getWidth(), getHeight() - responseCurve.getHeight());
            responseCurve.hide(true);
        }
            

        resized();

    };
    
    topBar.responseCurveToggle.setToggleState(true, juce::NotificationType::dontSendNotification);
    
    addAndMakeVisible(responseCurve);
    responseCurve.responseCurveChanged(true);

}

void RCAMKIISoundEffectsFilterAudioProcessorEditor::initialiseHighPassParams(RCAMKIISoundEffectsFilterAudioProcessor& p)
{
    /** Attachments & Sliders ... */
    highPassSliderAttachment = std::make_unique<apvts::SliderAttachment>(p.apvts, "HIGH_PASS_CUTOFF", HighPassSlider.getSlider());
    discreteHighPassAttachment = std::make_unique<apvts::SliderAttachment>(p.apvts, "DISC_HIGH_PASS", DiscHighPassSlider.getSlider());
    
    DiscHighPassSlider->setRange(1, 11, 1);
    DiscHighPassSlider->setTextValueSuffix("");
    DiscHighPassSlider->setNumDecimals(0);
    
    HighPassSlider->setRange(20, 20000);
    HighPassSlider->setTextValueSuffix(" Hz");
    HighPassSlider->setNumDecimals(1);
    
    highPassParams.setLabelText("HIGH PASS");
    
    /** Toggle lambdas ... */
    highPassControls.getToggleButton().onClick = [&]()
    {
        bool state = highPassControls.getToggleButton().getToggleState();
        highPassParams.setContinuous(state);

        p.isHighPassContinuous = state;
        p.highPassControlsChanged = true;
        
        for (auto& filter : p.getFilters())
        {
            filter.setHighPassMod(state);
            filter.reset();
        }

        p.updateFilters();
        responseCurve.responseCurveChanged(true);

    };
    
    highPassControls.getToggleButton().setToggleState(true, juce::NotificationType::dontSendNotification);
    
    highPassModToggle.getToggleButton().onClick = [&]()
    {
        int state = highPassModToggle.getToggleButton().getToggleState();

        for (auto& filter : p.getFilters())
        {
            filter.setHighPassMod(state);
            filter.reset();
        }
            
        p.getDummy().setHighPassMod(state);
        responseCurve.responseCurveChanged(true);

    };
    highPassModToggle.getToggleButton().setToggleState(true, juce::NotificationType::dontSendNotification);
}

void RCAMKIISoundEffectsFilterAudioProcessorEditor::initialiseLowPassParams(RCAMKIISoundEffectsFilterAudioProcessor& p)
{
    /** Attachments & Sliders ... */
    lowPassSliderAttachment = std::make_unique<apvts::SliderAttachment>(p.apvts, "LOW_PASS_CUTOFF", LowPassSlider.getSlider());
    discreteLowPassAttachment = std::make_unique<apvts::SliderAttachment>(p.apvts, "DISC_LOW_PASS", DiscLowPassSlider.getSlider());
    
    DiscLowPassSlider->setRange(1, 11, 1);
    DiscLowPassSlider->setTextValueSuffix("");
    DiscLowPassSlider->setNumDecimals(0);
    
    LowPassSlider->setRange(20, 20000);
    LowPassSlider->setTextValueSuffix(" Hz");
    LowPassSlider->setNumDecimals(1);
    
    lowPassParams.setLabelText("LOW PASS");

    /** Toggle lambdas ... */
    lowPassControls.getToggleButton().onClick = [&]()
    {
        bool state = lowPassControls.getToggleButton().getToggleState();
        lowPassParams.setContinuous(state);

        p.isLowPassContinuous = state;
        p.lowPassControlsChanged = true;
        
        for (auto& filter : p.getFilters())
        {
            filter.setLowPassMod(state);
            filter.reset();
        }
        
        p.updateFilters();
        responseCurve.responseCurveChanged(true);

    };
    
    lowPassControls.getToggleButton().setToggleState(true, juce::NotificationType::dontSendNotification);
    
    lowPassModToggle.getToggleButton().onClick = [&]()
    {
        int state = lowPassModToggle.getToggleButton().getToggleState();

        for (auto& filter : p.getFilters())
        {
            filter.setLowPassMod(state);
            filter.reset();
        }
                
        p.getDummy().setLowPassMod(state);
        p.updateFilters();

        responseCurve.responseCurveChanged(true);
    };
    
    lowPassModToggle.getToggleButton().setToggleState(true, juce::NotificationType::dontSendNotification);

}

void RCAMKIISoundEffectsFilterAudioProcessorEditor::initialiseMasterParams(RCAMKIISoundEffectsFilterAudioProcessor& p)
{
    
    /** Attachments & Sliders ... */
    ZInputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "Z_INPUT", ZInputSlider.getSlider());
    ZOutputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "Z_OUTPUT", ZOutputSlider.getSlider());
    outputGainSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.apvts, "OUTPUT_GAIN", OutputGainSlider.getSlider());
    
    ZInputSlider->setTextValueSuffix(" %");
    ZInputSlider->setNumDecimals(1);
    
    ZOutputSlider->setTextValueSuffix(" %");
    ZOutputSlider->setNumDecimals(1);
    
    OutputGainSlider->setRange(0, 20);
    OutputGainSlider->setNumDecimals(1);
    OutputGainSlider->setTextValueSuffix(" dB");
    
    MasterParams.setLabelText("MASTER");

}


RCAMKIISoundEffectsFilterAudioProcessorEditor::~RCAMKIISoundEffectsFilterAudioProcessorEditor()
{
}

//==============================================================================
void RCAMKIISoundEffectsFilterAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
}

void RCAMKIISoundEffectsFilterAudioProcessorEditor::resized()
{
    topBar.setBounds(getLocalBounds().removeFromTop(topBarHeight));
    
    juce::Rectangle<int> editorBounds = getLocalBounds().reduced(topBarHeight);
    
    juce::Rectangle<int> bottomHalfBounds = editorBounds.removeFromBottom(editorBounds.getHeight() / curveToParamsRatio);

    if (topBar.responseCurveToggle.getToggleState())
    {
        Container.setLabelHeight(10);
        responseCurve.setBounds(editorBounds);
        Container.setBounds(bottomHalfBounds);
    }
    else
    {
        Container.setLabelHeight(topBarHeight + 10);
        Container.setBounds(getLocalBounds());
    }
    
    if (Container.getWidth() < 400 || Container.getHeight() > 800)
        Container.setOrientation(false);
    else
        Container.setOrientation(true);

}
