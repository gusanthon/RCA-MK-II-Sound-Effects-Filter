/*
  ==============================================================================

    LabeledComponent.h
    Created: 30 Dec 2023 9:48:52pm
    Author:  Gus Anthon

  ==============================================================================
*/

#pragma once
#include "CustomLNF.h"


class LabeledComponent : public juce::Component
{
public:
    LabeledComponent()
    {
        label.setLookAndFeel(&mLNF);
        label.setJustificationType(Justification::centred);
        label.setFont(Font(GLOBAL_FONT, "Bold", 15.f));
    }

    void resized() override
    {
        auto bounds = getLocalBounds();
        label.setBounds(bounds.removeFromTop(labelHeight));
    }
    
    ~LabeledComponent()
    {
        label.setLookAndFeel(nullptr);
    }
    
    juce::Label& getLabel()
    {
        return label;
    }

    void setLabelHeight(int newLabelHeight)
    {
        labelHeight = newLabelHeight;
        label.setFont(juce::Font(newLabelHeight));
        resized();
    }
    
    int getLabelHeight()
    {
        return labelHeight;
    }
    

    juce::String getLabelText()
    {
        return labelText;
    }
    
    void setLabelText(juce::String newLabelText)
    {
        labelText = newLabelText;
        label.setText(labelText, juce::dontSendNotification);
    }
    

protected:
    
    int labelHeight = 30;
    juce::Label label;
    juce::String labelText;
    CustomLNF mLNF;
};




class CustomSlider : public juce::Slider
{
public:
    CustomSlider(juce::String paramLabel = "") : paramLabel_(paramLabel)
    {
        
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::TextBoxAbove, true, 60, 20);
        setLookAndFeel(&mLNF);
        
    }
    
    ~CustomSlider()
    {
        setLookAndFeel(nullptr);
    }
    
    juce::String getParamLabel()
    {
        return paramLabel_;
    }
    
     String getTextFromValue (double value) override
    {
        auto getText = [this] (auto val)
        {
            if (numDecimals > 0)
                return String (val, numDecimals);

            return String (roundToInt (val));
        };

        return getText (value) + getTextValueSuffix();
    }
    
    void setNumDecimals(int newNumDecimals)
    {
        numDecimals = newNumDecimals;
        repaint();
    }
    
    
private:
    int numDecimals = 0;
    
    CustomLNF mLNF;
    juce::String paramLabel_;
};





class SliderWithLabel : public LabeledComponent
{
public:
    SliderWithLabel(const juce::String& parameterLabel = "")
    {

        slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 72, 32);
        labelText = parameterLabel;
        addAndMakeVisible(slider);
        
        label.setLookAndFeel(&mLNF);
        label.setText(parameterLabel, juce::dontSendNotification);
        addAndMakeVisible(label);
        
    }

    
    ~SliderWithLabel()
    {
        slider.setLookAndFeel(nullptr);
        label.setLookAndFeel(nullptr);
    }
    
    void paint(juce::Graphics& g) override
    {
        if (slider.isMouseOverOrDragging() || label.isMouseOverOrDragging())
            setLabelHeight(31);
            
        else
            setLabelHeight(30);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();

            
        label.setBounds(bounds.removeFromTop(labelHeight));
        slider.setBounds(bounds);
        
        auto sliderHeight = slider.getY();
        
        //juce::Rectangle<int> r {}; init w/  slider.getY() + labelHeight ?
//        label.setBounds();
//        label.setBounds(bounds.removeFromTop(2 * labelHeight));
//        label.setBounds(bounds.removeFromBottom(getHeight()/2));
    }
    
    CustomSlider* operator->()
    {
        return &slider;
    }
    
    CustomSlider& getSlider()
    {
        return slider;
    }
    

private:
    
    CustomSlider slider;
    CustomLNF mLNF;
};
