/*
  ==============================================================================

    RCA_MKII_SEF.h
    Created: 30 Dec 2023 9:44:38pm
    Author:  Gus Anthon
 
    Based on https://dafx2020.mdw.ac.at/proceedings/papers/DAFx20in22_paper_39.pdf

  ==============================================================================
*/


#pragma once

#include "chowdsp_wdf.h"
#include <iostream>
#include <fstream>
#include <string>
#include <array>


using namespace chowdsp::wdft;


class RCA_MK2_SEF
{
public:
    RCA_MK2_SEF() = default;

    void prepare (float sampleRate)
    {
        fs = sampleRate;
        
        C_HP1.prepare(sampleRate);
        C_HP2.prepare(sampleRate);
        C_HPm1.prepare(sampleRate);
        C_HPm2.prepare(sampleRate);
        C_LP1.prepare(sampleRate);
        C_LPm1.prepare(sampleRate);

        L_HP1.prepare(sampleRate);
        L_HPm.prepare(sampleRate);
        L_LP1.prepare(sampleRate);
        L_LP2.prepare(sampleRate);
        L_LPm1.prepare(sampleRate);
        L_LPm2.prepare(sampleRate);
        
    }

    void reset()
    {
        C_HP1.reset();
        C_HP2.reset();
        C_HPm1.reset();
        C_HPm2.reset();
        C_LP1.reset();
        C_LPm1.reset();

        L_HP1.reset();
        L_HPm.reset();
        L_LP1.reset();
        L_LP2.reset();
        L_LPm1.reset();
        L_LPm2.reset();
    }

    void setOutputImpedance(float newZ)
    {
        if (outputImpedance != newZ)
        {
            outputImpedance = newZ;
            Rt.setResistanceValue(outputImpedance);
        }
    }

    void setInputImpedance(float newZ)
    {
        if (inputImpedance != newZ)
        {
            inputImpedance = newZ;
            Rin.setResistanceValue(inputImpedance);
        }
    }
    
    void setHighPassComponentValues(float C, float L)
    {
        C_HP1.setCapacitanceValue(C);
        C_HP2.setCapacitanceValue(C);
        L_HP1.setInductanceValue(L);

        if (! highPassMod)
        {
            float wc = 1e-8f;
            C = root2 / (k * wc);
            L = k / (2.0f * root2 * wc);
        }
        C_HPm1.setCapacitanceValue(C);
        C_HPm2.setCapacitanceValue(C);
        L_HPm.setInductanceValue(L);
        
    }

    void setHighPassCutoff(float newCutoff)
    {
        float wc = newCutoff * twoPi;
        float C = root2 / (k * wc);
        float L = k / (2.0f * root2 * wc);
        
        setHighPassComponentValues(C, L);
        highPassCutoff = newCutoff;
    }
    
    void setHighPassKnobPos(int pos)
    {
        jassert(pos <= HPVals.size() && pos >= 0);
        const auto& values = HPVals[pos - 1];
        setHighPassComponentValues(values.C, values.L);
    }
    
    /**
     * @TODO smooth C and L to mitigate clicks when switching knob pos
     **/
    void setLowPassComponentValues(float C, float L)
    {

        C_LP1.setCapacitanceValue(C);
        L_LP1.setInductanceValue(L);
        L_LP2.setInductanceValue(L);
        
        if (! lowPassMod)
        {
            float wc = 1e8f;
            C = (2.0f * root2) / (k * wc);
            L = (root2 * k) / wc;
        }

        C_LPm1.setCapacitanceValue(C);
        L_LPm1.setInductanceValue(L);
        L_LPm2.setInductanceValue(L);
    }
    
    void setLowPassCutoff(float newCutoff)
    {
        float wc = newCutoff * twoPi;
        float C = (2.0f * root2) / (k * wc);
        float L = (root2 * k) / wc;

        setLowPassComponentValues(C, L);
        
        lowPassCutoff = newCutoff;
    }
    
    void setLowPassKnobPos(int pos)
    {
        jassert(pos > 0 && pos <= LPVals.size());
        const auto& values = LPVals[pos - 1];
        setLowPassComponentValues(values.C, values.L);
    }

    void setLowPassMod(int mod)
    {
        if (lowPassMod != mod)
        {
            lowPassMod = mod;
            setLowPassCutoff(lowPassCutoff);
        }
    }

    void setHighPassMod(int mod)
    {
        if (highPassMod != mod)
        {
            highPassMod = mod;
            setHighPassCutoff(highPassCutoff);
        }
    }

    void setKVal(float kVal)
    {
        if (k != kVal)
            k = kVal;
    }

    inline float processSample (float x) noexcept
    {
        Vs.setVoltage(x);
        Vs.incident(S0.reflected());
        S0.incident(Vs.reflected());
        return voltage<float>(Rt);
    }
    
#define fftOrder 13
#define fftSize 2 << fftOrder
    
    void computeMagnitudeResponse(std::array<float, fftSize>& result) noexcept
    {
        
        for (int i = 0 ; i < impulse.size(); ++i)
            result[i] = processSample(impulse[i]);

        fft.performFrequencyOnlyForwardTransform(result.data(), true);

        reset();
    }
    
    /**
     * Used for validating frequency response data in Python
     */
    void saveResponseToCSV(const std::array<float, fftSize>& response, const std::string& filename)
    {
        
        std::ofstream file;

        file.open(filename);

        for (auto it = response.begin(); it != response.end(); ++it)
          {
              file << *it;

              if (std::next(it) != response.end())
                  file << ",";
          }
        
        file.close();
        std::cout << "Data saved to " + filename << std::endl;
    }

    
    float getHighPassCutoff() {return highPassCutoff;}
    float getLowPassCutoff() {return lowPassCutoff;}
   
    
private:
    
    juce::dsp::FFT fft {fftOrder};
    
    float i = juce::Decibels::decibelsToGain(1e-12);
    const std::array<float, fftSize> impulse = {i};

    const float root2 = juce::MathConstants<float>::sqrt2;
    const float twoPi = juce::MathConstants<float>::twoPi;
    
    int highPassMod = 1;
    int lowPassMod = 1;
    
    float highPassCutoff = 20.f;
    float lowPassCutoff = 20000.f;

    float inputImpedance = 560;
    float outputImpedance = 560;

    float k = 560.0f;

    float fs = 48000;
        
    ResistorT<float> Rt {outputImpedance};
    InductorT<float> L_LPm2 {1.0e-3f, double (48000)};

    WDFSeriesT<float, decltype(L_LPm2), decltype(Rt)> S8 {L_LPm2, Rt};
    CapacitorT<float> C_LPm1 {1.0e-8f, double (48000)};

    WDFParallelT<float, decltype(C_LPm1), decltype(S8)> P4 {C_LPm1, S8};
    InductorT<float> L_LPm1 {1.0e-3f, double (48000)};

    WDFSeriesT<float, decltype(L_LPm1), decltype(P4)> S7 {L_LPm1, P4};
    InductorT<float> L_LP2 {1.0e-3f, double (48000)};

    WDFSeriesT<float, decltype(L_LP2), decltype(S7)> S6 {L_LP2, S7};
    CapacitorT<float> C_LP1 {1.0e-8f, double (48000)};

    WDFParallelT<float, decltype(C_LP1), decltype(S6)> P3 {C_LP1, S6};
    InductorT<float> L_LP1 {1.0e-3f, double (48000)};

    WDFSeriesT<float, decltype(L_LP1), decltype(P3)> S5 {L_LP1, P3};
    CapacitorT<float> C_HP2 {1.0e-8f, double (48000)};

    WDFSeriesT<float, decltype(C_HP2), decltype(S5)> S4 {C_HP2, S5};
    InductorT<float> L_HP1 {1.0e-3f, double (48000)};

    WDFParallelT<float, decltype(L_HP1), decltype(S4)> P2 {L_HP1, S4};
    CapacitorT<float> C_HP1 {5.0e-8f, double (48000)};

    WDFSeriesT<float, decltype(C_HP1), decltype(P2)> S3 {C_HP1, P2};
    CapacitorT<float> C_HPm2 {5.0e-8f, double (48000)};

    WDFSeriesT<float, decltype(C_HPm2), decltype(S3)> S2 {C_HPm2, S3};
    InductorT<float> L_HPm {1.0e-3f, double (48000)};

    WDFParallelT<float, decltype(L_HPm), decltype(S2)> P1 {L_HPm, S2};
    CapacitorT<float> C_HPm1 {5.0e-8f, double (48000)};

    WDFSeriesT<float, decltype(C_HPm1), decltype(P1)> S1 {C_HPm1, P1};
    ResistorT<float> Rin {inputImpedance};

    WDFSeriesT<float, decltype(Rin), decltype(S1)> S0 {Rin, S1};
    IdealVoltageSourceT<float, decltype(S0)> Vs {S0};
    
    struct ComponentValues
    {
        float C;
        float L;
    };

    std::vector<ComponentValues> HPVals =
    {
        {99999, 99999},
        {1.6e-6, 255.6e-3},
        {1.15e-6, 176.9e-3},
        {0.8e-6, 126.4e-3},
        {0.57e-6, 90.11e-3},
        {0.4e-6, 63.9e-3},
        {0.272e-6, 44.56e-3},
        {0.2e-6, 31.79e-3},
        {0.15e-6, 21.77e-3},
        {0.1e-6, 15.63e-3},
        {0.069e-6, 11.18e-3}
    };
    
    std::vector<ComponentValues> LPVals =
    {
        {3.22e-6, 511.1e-3},
        {2.3e-6, 365.2e-3},
        {1.6e-6, 255.6e-3},
        {1.15e-6, 178.6e-3},
        {0.8e-6, 126.4e-3},
        {0.57e-6, 90.02e-3},
        {0.4e-6, 63.54e-3},
        {0.272e-6, 45.08e-3},
        {0.2e-6, 32.13e-3},
        {0.15e-6, 22.38e-3},
        {1e-10, 1e-10}
    };
    
    /** {fc : {C, L}} */
    
    std::map<int, ComponentValues> HP_map =
    {
        {0, {99999, 99999}},
        {175, {1.6e-6, 255.6e-3}},
        {248, {1.15e-6, 176.9e-3}},
        {352, {0.8e-6, 126.4e-3}},
        {497, {0.57e-6, 90.11e-3}},
        {699, {0.4e-6, 63.9e-3}},
        {1002, {0.272e-6, 44.56e-3}},
        {1411, {0.2e-6, 31.79e-3}},
        {2024, {0.15e-6, 21.77e-3}},
        {2847, {0.1e-6, 15.63e-3}},
        {3994, {0.069e-6, 11.18e-3}}
    };
    
    std::map<int, ComponentValues> LP_map =
    {
        {175, {3.22e-6, 511.1e-3}},
        {245, {2.3e-6, 365.2e-3}},
        {350, {1.6e-6, 255.6e-3}},
        {499, {1.15e-6, 178.6e-3}},
        {703, {0.8e-6, 126.4e-3}},
        {996, {0.57e-6, 90.02e-3}},
        {1408, {0.4e-6, 63.54e-3}},
        {1989, {0.272e-6, 45.08e-3}},
        {2803, {0.2e-6, 32.13e-3}},
        {3992, {0.15e-6, 22.38e-3}},
        {999999, {1e-10, 1e-10}}
    };
    
};


