/*
  ==============================================================================

    CustomLNF.h
    Created: 30 Dec 2023 9:49:42pm
    Author:  Gus Anthon

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;

#define GLOBAL_FONT "Silom"


class CustomLNF : public LookAndFeel_V4
{
public:
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        
        auto radius = jmin(width / 2, height / 2) - 5.0f;

        auto centreX = x + width * 0.5f;
        auto centreY = y + height * 0.5f;

        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        Path foregroundArc;
        foregroundArc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.setColour(Colours::antiquewhite);
        g.strokePath(foregroundArc, PathStrokeType(radius * 0.1f, PathStrokeType::curved, PathStrokeType::rounded));

        auto innerRadius = radius * 0.9f;
        auto innerX = centreX - innerRadius;
        auto innerY = centreY - innerRadius;

        ColourGradient gradient(Colour(0xFF333333), innerX, innerY, Colour(0xFFCCCCCC), innerX + innerRadius * 2.0f, innerY + innerRadius * 2.0f, false);

        juce::Colour ptr_colour = juce::Colours::antiquewhite;

        if (slider.isMouseOverOrDragging())
        {
            gradient.multiplyOpacity(.75);

            ptr_colour = ptr_colour.withAlpha(0.25f);
        }
        
        g.setGradientFill(gradient);
        g.fillEllipse(innerX, innerY, innerRadius * 2.0f, innerRadius * 2.0f);

        g.setColour(ptr_colour);
                
        Path p;
        auto pointerLength = radius * .8f;
        auto pointerThickness = radius * .05f;

        p.addRoundedRectangle(-pointerThickness, -radius - 1, 2 * pointerThickness, pointerLength, 2);
        p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));
        g.fillPath(p);

    }
    

    void drawLabel(Graphics& g, Label& label) override
    {

        g.setColour(label.findColour(Label::textColourId));
        const String& text = label.getText();

        Rectangle<int> textArea(label.getBorderSize().subtractedFrom(label.getLocalBounds()));

        juce::Font fontToDraw(GLOBAL_FONT, "Bold", 18);
        
        g.setFont(fontToDraw);
        g.drawFittedText(text, textArea, label.getJustificationType(), 1);
        
        g.setColour(juce::Colours::white);
        float cornerSize = 8.0f;
        g.drawRoundedRectangle(textArea.toFloat(), cornerSize, .5f);
    }
    

};



class ToggleLNF : public LookAndFeel_V4
{
public:
    ToggleLNF(bool vertical = true) : vertical_(vertical) {}
    
    void drawToggleButton(Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        /**@TODO this needs cleaning up **/
        
        if (vertical_)
        {
            const int width = button.getWidth() ;
            const int height = button.getHeight() * .5;

            const int xPos = 0;
            const int yPos = 0;

            auto backgroundColor = juce::Colours::grey.withAlpha(.65f);
            auto r = juce::Colours::red;
            
            if (shouldDrawButtonAsDown)
                r = r.brighter();

            if (shouldDrawButtonAsHighlighted)
            {
                backgroundColor = backgroundColor.withAlpha(.4f);
                r = r.withAlpha(.7f);
            }

            g.setColour(backgroundColor);
            g.fillRect(xPos, yPos, width, height);
            
            g.setFont(Font(GLOBAL_FONT, "Bold", fontSize));

            if (button.getToggleState())
            {
                g.setColour(r);
                g.fillRect(xPos, yPos, width, height / 2);

                g.setColour(juce::Colours::white);
                g.drawFittedText(onText, xPos, yPos, width, height / 2, Justification::centred, 1);
            }

            else
            {
                g.setColour(r);
                g.fillRect(xPos, yPos + height / 2, width, height / 2);

                g.setColour(juce::Colours::white);
                g.drawFittedText(offText, xPos, yPos + height / 2, width, height / 2, Justification::centred, 1);
            }

            g.setColour(juce::Colours::black);
            g.drawRect(xPos, yPos, width, height);

        }
        else
        {
            const int width = button.getWidth() * .5;
            const int height = button.getHeight();

            const int xPos = 0;
            const int yPos = 0;

            auto backgroundColor = juce::Colours::grey.withAlpha(.65f);
            auto r = juce::Colours::red;

            if (shouldDrawButtonAsDown)
                r = r.brighter();
            
            if (shouldDrawButtonAsHighlighted)
            {
                backgroundColor = backgroundColor.withAlpha(.4f);
                r = r.withAlpha(.7f);
            }


            g.setColour(backgroundColor);
            g.fillRect(xPos, yPos, width, height);

            g.setFont(Font(GLOBAL_FONT, "Bold", fontSize));

            if (button.getToggleState())
            {
                g.setColour(r);
                g.fillRect(xPos, yPos, width / 2, height);

                g.setColour(juce::Colours::white);
                g.drawFittedText(onText, xPos, yPos, width / 2, height, Justification::centred, 1);
            }
            else
            {
                g.setColour(r);
                g.fillRect(xPos + width / 2, yPos, width / 2, height);

                g.setColour(juce::Colours::white);
                g.drawFittedText(offText, xPos + width / 2, yPos, width / 2, height, Justification::centred, 1);
            }

            g.setColour(juce::Colours::black);
            g.drawRect(xPos, yPos, width, height);

        }
      
    }
protected:
    bool vertical_;
    float fontSize = 15.f;
    juce::String onText {"ON"};
    juce::String offText {"OFF"};
};


class ControlsLNF : public ToggleLNF
{
public:
    ControlsLNF(bool vertical = true)
    {
        fontSize = 13.f;
        onText = "CONTINUOUS";
        offText = "DISCRETE";
    }
};


