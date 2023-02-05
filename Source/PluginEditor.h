/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class newPluginAudioProcessorEditor  : public juce::AudioProcessorEditor, 
                                      public Slider::Listener
{
public:
    newPluginAudioProcessorEditor (newPluginAudioProcessor&);
    ~newPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    //update parameters from change in slider values
    void sliderValueChanged(Slider* slider) override;

private:

    Slider gainSlider;
    Slider freqSlider;
    Slider cutoffSlider;
    Slider resonanceSlider;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    newPluginAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (newPluginAudioProcessorEditor)
};
