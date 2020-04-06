/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Lecture8AudioProcessor::Lecture8AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true) //delay will be applied to each channel here
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    delayTime = new AudioParameterFloat("delayTime", "Delay Time (0-1s)", 0.0f, 1.0f, 0.33f); // values between 0 and 1, initial value = 0.33
    addParameter(delayTime);

    feedback = new AudioParameterFloat("feedback", "Feedback (0-0.95)", 0.0f, 0.95f, 0.0f); // values between 0 and .95, initial value = 0
    addParameter(feedback);
}

Lecture8AudioProcessor::~Lecture8AudioProcessor()
{
}

//==============================================================================
const String Lecture8AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Lecture8AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Lecture8AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Lecture8AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Lecture8AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Lecture8AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Lecture8AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Lecture8AudioProcessor::setCurrentProgram (int index)
{
}

const String Lecture8AudioProcessor::getProgramName (int index)
{
    return {};
}

void Lecture8AudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Lecture8AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

 
    delayBufferLength = (int)(sampleRate);  //stores delay buffer length in samples
    delayBuffer.setSize(2, delayBufferLength); //set size (will hold one second of audio over two channels 
    delayBuffer.clear(); //resests contets to 0
    //determine where read index is in relevance to write index
    readIndex = (int)(writeIndex - (delayTime->get() * getSampleRate()) //converting to samples by mutltiplying w sample rate
        + delayBufferLength) % delayBufferLength; 

}

void Lecture8AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Lecture8AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void Lecture8AudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    //Delay processing code, copy of read and write index (maintains vals stored here in each channel.
    //scope is limited to variable block
    int tempReadIndex = 0;
    int tempWriteIndex = 0;


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        //setting up ponter to delay buffer...
        auto* channelData = buffer.getWritePointer(channel);
        auto* delayData = delayBuffer.getWritePointer(channel);

        float wetMix = 0.5f; //temp local variable (set up const for wet mix)

        //assigning to local temp variables
        tempReadIndex = readIndex;
        tempWriteIndex = writeIndex;

        //Audio processing...
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            auto in = channelData[i]; //retrives input sample from buffer
            auto out = (in + (wetMix * delayData[tempReadIndex])); //computes what out put sample shouold be 

            delayData[tempWriteIndex] = in + (delayData[tempReadIndex] * feedback->get());

            /*++ operator increments value by 1, check to see 
            if more than or equal to, if =>, resets to 0*/
            if (++tempReadIndex >= delayBufferLength)
              tempReadIndex = 0;

            if (++tempReadIndex >= delayBufferLength)
                tempWriteIndex = 0;

            channelData[i] = out;
        }
    }

    readIndex = tempReadIndex;
    writeIndex = tempWriteIndex;
}

//==============================================================================
bool Lecture8AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Lecture8AudioProcessor::createEditor()
{
    return new GenericAudioProcessorEditor (*this);
}

//==============================================================================
void Lecture8AudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Lecture8AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Lecture8AudioProcessor();
}
