/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
newPluginAudioProcessor::newPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

newPluginAudioProcessor::~newPluginAudioProcessor()
{
}

//==============================================================================
const juce::String newPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool newPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool newPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool newPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double newPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int newPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int newPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void newPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String newPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void newPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void newPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    fs = sampleRate; // Obtain the sample rate from the plugin host (or DAW) when the application starts
    filterIn = 0.0;
    cutoff = 400.0;
}

void newPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool newPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void newPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {

        // This is our input buffer. (To be used later)
        auto input = buffer.getReadPointer(channel);

        // This is the pointer our output buffer.
        // We will use this variable to send data to our speakers.
        auto* output = buffer.getWritePointer(channel);

        calculateFilterCoefs();
        setFilterConstants();

        // Extra variable for clarity.
        float nonLimitedOutput = 0.0;

        float filterOutput = 0.0;
        float prevIn = 0.0;
        float prevOut = 0.0;

        // Here, we only do processing on the left channel and copy everything to the right channel
        if (channel == 0)
        {
            // Loop over all the samples in our buffer
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {

                nonLimitedOutput = gain * (sin(2.0 * double_Pi * frequency * n / fs));

                    //+ fourierSquare(199, frequency, fs, n);

                    //+ sin(3 * 2.0 * double_Pi * frequency * n / fs) / 3);
                 /* + sin(5 * 2.0 * double_Pi * frequency * n / fs) / 5
                    + sin(7 * 2.0 * double_Pi * frequency * n / fs) / 7
                    + sin(9 * 2.0 * double_Pi * frequency * n / fs) / 9
                    + sin(11 * 2.0 * double_Pi * frequency * n / fs) / 11
                    + sin(13 * 2.0 * double_Pi * frequency * n / fs) / 13
                    + sin(15 * 2.0 * double_Pi * frequency * n / fs) / 15
                    + sin(17 * 2.0 * double_Pi * frequency * n / fs) / 17
                    + sin(19 * 2.0 * double_Pi * frequency * n / fs) / 19
                    + sin(21 * 2.0 * double_Pi * frequency * n / fs) / 21); */

                filterOutput = moogfilterSimpleModel(nonLimitedOutput, prevIn, prevOut);

                //update filter input values for next sample point
                prevIn = nonLimitedOutput;
                prevOut = filterOutput;

                output[i] = limit(filterOutput, -1.0, 1.0);

                ++n; // Increment sample index for sinusoid
            }
        } else {

            // Copy the left channel to the right channel
            for (int i = 0; i < buffer.getNumSamples(); ++i)
                output[i] = buffer.getWritePointer(channel - 1)[i];
        }

    }
}

//==============================================================================
bool newPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* newPluginAudioProcessor::createEditor()
{
    return new newPluginAudioProcessorEditor (*this);
}

//==============================================================================
void newPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void newPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new newPluginAudioProcessor();
}

// Implementation of the limiter
float newPluginAudioProcessor::limit(float val, float min, float max)
{
    if (val < min)
        return min;
    else if (val > max)
        return max;
    else
        return val;
}

float newPluginAudioProcessor::fourierSquare(int sinewaves, float freq, float fs, float sample)
{
    float val = 0.0;

    for (int i = 3; i < sinewaves; i + 2) {
        val = + sin(i * 2.0 * double_Pi * freq * sample / fs) / i;
    }

    return val;
}

float newPluginAudioProcessor::moogfilterSimpleModel(float input, float prevIn, float prevOut)
{
    const int numFilters = 4;

    //previous In and Out values of 
    float uStatesIn[numFilters+1]; 
    float uStatesOut[numFilters+1];
   
    //calculateFilterCoefs();
    //setFilterConstants();

    float inputMod = tanh(input - numFilters * resonance * (prevOut - gainCompensation * input));
    float moogOut;
    
    uStatesIn[1] = inputMod;
    uStatesIn[0] = prevIn;
    uStatesOut[0] = prevOut;

    for (int i = 1; i < numFilters+1; ++i) {

        //uStatesIn[i] = tanh(uStatesIn[i] - 4 * resonance * (uStatesOut[i - 1] - gainCompensation * uStatesIn[i]));

        uStatesOut[i] = b0 * uStatesIn[i] + b1 * uStatesIn[i - 1] - (a1 * uStatesOut[i - 1]);

        if (i <= numFilters) {
            uStatesIn[i + 1] = uStatesOut[i];
        }

    }

    moogOut = uStatesOut[numFilters];
    return moogOut;

}

void newPluginAudioProcessor::calculateFilterCoefs()
{
    float g = 2 * double_Pi * cutoff / (fs * 2);

    b0 = g / 1.3;
    b1 = g * 0.3 / 1.3;

    a1 = 1 - g;
}

void newPluginAudioProcessor::setFilterConstants()
{
    gainCompensation = 0.5;
}