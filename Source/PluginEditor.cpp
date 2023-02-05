/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
newPluginAudioProcessorEditor::newPluginAudioProcessorEditor (newPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    gainSlider.setRange(0, 1, 0.01);
    gainSlider.setValue(0.5);

    freqSlider.setRange(20.00, 400.00, 0.01);
    freqSlider.setValue(220.00);

    cutoffSlider.setRange(200, 1200, 0.1);
    cutoffSlider.setValue(200);

    resonanceSlider.setRange(0, 1, 0.01);
    resonanceSlider.setValue(0.0);

    gainSlider.addListener(this);
    freqSlider.addListener(this);
    cutoffSlider.addListener(this);
    resonanceSlider.addListener(this);

    addAndMakeVisible(gainSlider);
    addAndMakeVisible(freqSlider);
    addAndMakeVisible(cutoffSlider);
    addAndMakeVisible(resonanceSlider);

    setSize (600, 400);
}

newPluginAudioProcessorEditor::~newPluginAudioProcessorEditor()
{
}

//==============================================================================
void newPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void newPluginAudioProcessorEditor::resized()
{

    gainSlider.setBounds(0, 0, getHeight()-40, getWidth());

    freqSlider.setBounds(0, 30, getHeight(), getWidth());

    cutoffSlider.setBounds(0, 60, getHeight()+40, getWidth());

    resonanceSlider.setBounds(0, 90, getHeight() + 80, getWidth());

}

void newPluginAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
    /* Adding parameter control [7]: Apply the slider value to the Processor parameter

            (Note: if you're looking for step [6], it's in the PluginProcessor.h file :) )

            Now that we created a function to set the gain of the Processor, let's apply the slider value to the gain.
            Obtaining the slider value happens through the Slider class' "getValue()" function.

     */

    
    if (slider == &gainSlider)
        audioProcessor.setGain(gainSlider.getValue());

    if (slider == &freqSlider)
        audioProcessor.setFreq(freqSlider.getValue());

    if (slider == &cutoffSlider)
        audioProcessor.setCutoff(cutoffSlider.getValue());

    if (slider == &resonanceSlider)
        audioProcessor.setResonance(resonanceSlider.getValue());
}

