/*
  ==============================================================================

    ResponseCurveComponent.h
    Created: 30 Dec 2023 9:49:24pm
    Author:  Gus Anthon
 
    Adapted from https://github.com/matkatmusic/SimpleEQ

  ==============================================================================
*/

#pragma once


#include "LabeledComponent.h"
#include "PluginProcessor.h"

class ResponseCurveComponent : public LabeledComponent,
                                      juce::AudioProcessorParameter::Listener,
                                      juce::Timer

{
public:
    ResponseCurveComponent(RCAMKIISoundEffectsFilterAudioProcessor& proc) : proc_(proc)
    {

        const auto& params = proc.getParameters();

        for (auto& param : params)
            param->addListener(this);
        
        updateMags();
        updateResponseCurve();
        
        startTimerHz(30);

    }
    
    void updateMags()
    {
        gain = proc_.getCurrentGain();
        proc_.computeMagnitudeResponse(mags);

    }
    
    
    void updateResponseCurve()
    {

        juce::MessageManager::callAsync([&]()
        {
            repaint();

            auto bounds = getAnalysisArea();
            auto left = bounds.getX();
            auto width = bounds.getWidth();

            const auto fs = proc_.getSampleRate();
            
            float bottom = bounds.getBottom();
            float top = bounds.getY();
            
            responseCurve.clear();
            responseCurve.startNewSubPath(left, juce::jmap(mags[0], 0.f, 2.f, bottom, top));

            for (int i = 0; i < n2; ++i)
            {
                float freq = i == 0 ? 20 : (i * fs / n2);
                
                float logFreq = juce::jmap(std::log10(freq), log20, log20k, 0.f, 1.f);
                
                float xVal = left + width * logFreq;
                xVal = std::clamp(xVal, float(left), float(left + width));

                auto mag = jmap(mags[i] * gain, 0.f, 2.f, float(bottom), float(top));
                if (mag < top)
                    mag = top;

                responseCurve.lineTo(xVal, mag);
                
            }
            needsUpdate = false;
        });
        
    }
    
    
    
    void responseCurveChanged(bool b)
    {
        needsUpdate = b;
    }

    void hide(bool b)
    {
        isHidden = b;
    }
    
    
private:
    
    void timerCallback() override
    {
        if (needsUpdate && ! isHidden)
        {

            updateMags();
            updateResponseCurve();
        }
        
    }

    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override
    {
        needsUpdate = true;
    }

    void parameterValueChanged (int parameterIndex, float newValue) override
    {
        needsUpdate = true;
    }

    
    void resized() override
    {
        updateResponseCurve();
    }
    
    
    juce::Rectangle<int> getAnalysisArea()
    {
        auto bounds = getLocalBounds();
        int rectWidth = bounds.getWidth() * 0.8;
        int rectHeight = bounds.getHeight() * 0.8;
        int x = bounds.getWidth() / 2 - rectWidth / 2;
        int y = bounds.getHeight() / 2 - rectHeight / 2;

        juce::Rectangle<int> centeredBounds(x, y, rectWidth, rectHeight);
        return centeredBounds;
    }
    

    

    void paint(juce::Graphics& g) override
    {

        g.setColour(Colours::white);
        g.strokePath(responseCurve, PathStrokeType(2.f));
        
        auto bounds = getAnalysisArea();
        
        g.setColour(juce::Colours::green);

        int rectWidth = bounds.getWidth();
        int rectHeight = bounds.getHeight();
        
        int x = bounds.getWidth() / 2 - rectWidth / 2;
        int y = bounds.getHeight() / 2 - rectHeight / 2;

        juce::Rectangle<int> centeredBounds(x, y, rectWidth, rectHeight);

        drawBackgroundGrid(g);
        drawTextLabels(g);

    }
    
    void drawTextLabels(juce::Graphics &g)
    {
        using namespace juce;
        g.setColour(Colours::lightgrey);
        const int fontHeight = 10;
        g.setFont(fontHeight);
        
        auto renderArea = getAnalysisArea();
        auto left = renderArea.getX();
        
        auto top = renderArea.getY();
        auto bottom = renderArea.getBottom();
        auto width = renderArea.getWidth();
        
        auto freqs = getFrequencies();
        auto xs = getXs(freqs, left, width);
        
        for( int i = 0; i < freqs.size(); ++i )
        {
            auto f = freqs[i];
            auto x = xs[i];

            bool addK = false;
            String str;
            if( f > 999.f )
            {
                addK = true;
                f /= 1000.f;
            }

            str << f;
            if( addK )
                str << "k";
            str << "Hz";
            
            auto textWidth = g.getCurrentFont().getStringWidth(str);

            Rectangle<int> r;

            r.setSize(textWidth, fontHeight);
            r.setCentre(x, 0);
            r.setY(2);
            
            g.drawFittedText(str, r, juce::Justification::centred, 1);
        }
        
        auto gain = getGains();

        for( auto gDb : gain )
        {
            auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
            
            String str;
            if( gDb > 0 )
                str << "+";
            str << gDb/2.f;
//            str << gDb;
            
            auto textWidth = g.getCurrentFont().getStringWidth(str);
            
            Rectangle<int> r;
            r.setSize(textWidth, fontHeight);
            r.setX(getWidth() - textWidth);
            r.setCentre(r.getCentreX(), y);
            
            g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::lightgrey );
            
//            str.clear();
//            str << (gDb /2.f);
            
            g.drawFittedText(str, r, juce::Justification::centredLeft, 1);

//            str.clear();
//            str << (gDb - 24.f);

            r.setX(1);
            textWidth = g.getCurrentFont().getStringWidth(str);
            r.setSize(textWidth, fontHeight);
            g.setColour(Colours::lightgrey);
            g.drawFittedText(str, r, juce::Justification::centredLeft, 1);
        }
    }
    
    
    std::vector<float> getFrequencies()
    {
        return std::vector<float>
        {
            20, /*30, 40,*/ 50, 100,
            200, /*300, 400,*/ 500, 1000,
            2000, /*3000, 4000,*/ 5000, 10000,
            20000
        };
    }
    
    std::vector<float> getGains()
    {
        return std::vector<float>
        {
            -24, -12, 0, 12, 24
        };
    }
    
    std::vector<float> getXs(const std::vector<float> &freqs, float left, float width)
    {
        std::vector<float> xs;
        for( auto f : freqs )
        {
            auto normX = juce::mapFromLog10(f, 20.f, 20000.f);
            xs.push_back( left + width * normX );
        }
        return xs;
    }
    
    void drawBackgroundGrid(juce::Graphics &g)
    {
        using namespace juce;
        auto freqs = getFrequencies();
        
        auto renderArea = getAnalysisArea();
        auto left = renderArea.getX();
        auto right = renderArea.getRight();
        auto top = renderArea.getY();
        auto bottom = renderArea.getBottom();
        auto width = renderArea.getWidth();
        
        auto xs = getXs(freqs, left, width);
        
        g.setColour(Colours::dimgrey);
        for( auto x : xs )
        {
            g.drawVerticalLine(x, top, bottom);
        }
        
        auto gain = getGains();
        
        for( auto gDb : gain )
        {
            auto y = jmap(gDb, -24.f, 24.f, float(bottom), float(top));
            
            g.setColour(gDb == 0.f ? Colour(0u, 172u, 1u) : Colours::darkgrey );
            g.drawHorizontalLine(y, left, right);
        }
    }
    
    const float log20 = std::log10(20.f);
    const float log20k = std::log10(20000.f);
    
    bool needsUpdate = true;
    bool isHidden = false;
    
    float gain;
    
    RCAMKIISoundEffectsFilterAudioProcessor& proc_;
    juce::Path responseCurve;
    
    std::array<float, fftSize> mags;
    const int n2 = (int) mags.size() / 2;

};

