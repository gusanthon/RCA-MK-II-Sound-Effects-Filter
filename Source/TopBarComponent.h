/*
  ==============================================================================

    TopBarComponent.h
    Created: 30 Dec 2023 9:49:30pm
    Author:  Gus Anthon

  ==============================================================================
*/

#pragma once


#pragma once
#include "JuceHeader.h"
#include "CustomLNF.h"

class TopBarComponent : public Component
{
public:
    TopBarComponent()
    {
        
        pluginNameLabel.setText(pluginName, dontSendNotification);
        pluginNameLabel.setFont(Font(GLOBAL_FONT, 20, juce::Font::plain));
        pluginNameLabel.setJustificationType(Justification::centredLeft);
        addAndMakeVisible(pluginNameLabel);

        toggleLabel.setText("SHOW CURVE", dontSendNotification);
        toggleLabel.setFont(Font(GLOBAL_FONT, 20, juce::Font::plain));
        addAndMakeVisible(toggleLabel);
        
        responseCurveToggle.setLookAndFeel(&tlnf);
        addAndMakeVisible(responseCurveToggle);
        resized();
    }

    ~TopBarComponent()
    {
        pluginNameLabel.setLookAndFeel(nullptr);
        responseCurveToggle.setLookAndFeel(nullptr);
        toggleLabel.setLookAndFeel(nullptr);
    }

    void paint(juce::Graphics& g) override
    {

        g.fillAll(juce::Colour::fromRGB(20,20,20));
                
        g.setColour(juce::Colours::white.darker());
        juce::Rectangle<int> bounds = getLocalBounds();
        g.drawRect(bounds, 1.);
        
    }


    void resized() override
    {
        if (getWidth() < 400)
            pluginNameLabel.setVisible(false);
        else
            pluginNameLabel.setVisible(true);
        
        int margin = 10;
        int logoSize = 20;
        int presetButtonWidth = 140;

        int labelX = margin + logoSize + margin;
        int labelWidth = getWidth() - labelX - presetButtonWidth - margin;
        pluginNameLabel.setBounds(labelX, margin, labelWidth, logoSize);

        int responseCurveToggleWidth = 100;
        int toggleWidth = getWidth() - responseCurveToggleWidth;
        responseCurveToggle.setBounds(toggleWidth, margin, responseCurveToggleWidth, logoSize);
        
        int labelToToggleMargin = 10;
        
        toggleLabel.setBounds(toggleWidth - labelToToggleMargin- responseCurveToggleWidth, margin, labelWidth, logoSize);
        
    }
    
    juce::ToggleButton responseCurveToggle;

private:
    juce::String pluginName {"RCA MK II SOUND EFFECTS FILTER"};

    Label pluginNameLabel;
    Label toggleLabel;

    CustomLNF mLNF;
    
    ToggleLNF tlnf = ToggleLNF(false);

};
