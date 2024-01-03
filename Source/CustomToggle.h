/*
  ==============================================================================

    CustomToggle.h
    Created: 2 Jan 2024 6:39:32pm
    Author:  Gus Anthon

  ==============================================================================
*/

#pragma once
#include "LabeledComponent.h"

class CustomToggle : public LabeledComponent
{
public:
    CustomToggle(const juce::String paramLabel = "")
    {
        labelText = paramLabel;
        toggle.setLookAndFeel(&tlnf);
        addAndMakeVisible(toggle);
        
        label.setText(paramLabel, dontSendNotification);
        addAndMakeVisible(label);
    }
    
    ~CustomToggle()
    {
        toggle.setLookAndFeel(nullptr);
    }
    
    
    void resized() override
    {

        float toggleWidthRatio = 0.6f;

        int toggleWidth = static_cast<int>(getWidth() * toggleWidthRatio);
        int xPosition = (getWidth() - toggleWidth) / 2;

        toggle.setBounds(xPosition, (getHeight() - labelHeight) / 2, toggleWidth, 100);
        
        LabeledComponent::resized();

    }

    void paint(juce::Graphics& g) override
    {
        if (toggle.isMouseOverOrDragging() || label.isMouseOverOrDragging())
            setLabelHeight(31);
        else
            setLabelHeight(30);
    }
    
    juce::Button& getToggleButton()
    {
        return toggle;
    }
    
protected:
    juce::ToggleButton toggle;
    ToggleLNF tlnf;
};





class ControlsToggle : public CustomToggle
{
public:
    ControlsToggle(const juce::String paramLabel = "") : CustomToggle(paramLabel)
    {
        toggle.setLookAndFeel(&clnf);
    }
    ~ControlsToggle()
    {
        toggle.setLookAndFeel(nullptr);
    }
    
private:
    ControlsLNF clnf;
};
