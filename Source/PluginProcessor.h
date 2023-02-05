/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class newPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    newPluginAudioProcessor();
    ~newPluginAudioProcessor() override;

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

    // Limiter to keep the output signal between -1 and 1. Arguments are: input value, minimum value and maximum value
    float limit(float val, float min, float max);

    float fourierSquare(int sinewaves, float freq, float fs, float sample);

    float moogfilter(float value, float prevIn, float prevOut);
    
    float moogfilterSimpleModel(float value, float prevIn, float prevOut);

    void calculateFilterCoefs();
    void setFilterConstants();

// functions for sliders in PluginEditor
    void setGain(double gainToSet) { gain = gainToSet; };

    void setFreq(double freqToSet) { frequency = freqToSet; }

    void setCutoff(float cutoffToSet) { cutoff = cutoffToSet; }

    void setResonance(float resonanceToSet) { resonance = resonanceToSet; }


private:

    float filterIn;
    float cutoff;

    double fs; // to be obtained from the "prepareToPlay()" function

    double gain = 0.5;

    // Sample index for the sinusoid
    unsigned long n = 0;

    float frequency = 220.00;

    float b0 = 0.0;
    float b1 = 0.0;
    float a1 = 0.0;
 
    float resonance = 0.2;
    float gainCompensation = 0.0;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (newPluginAudioProcessor)
};
