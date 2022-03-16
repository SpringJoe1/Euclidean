/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"





juce::String getMidiMessageDescription(const juce::MidiMessage& m)
{
    if (m.isNoteOn())           return "Note on " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
    if (m.isNoteOff())          return "Note off " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3);
    if (m.isProgramChange())    return "Program change " + juce::String(m.getProgramChangeNumber());
    if (m.isPitchWheel())       return "Pitch wheel " + juce::String(m.getPitchWheelValue());
    if (m.isAftertouch())       return "After touch " + juce::MidiMessage::getMidiNoteName(m.getNoteNumber(), true, true, 3) + ": " + juce::String(m.getAfterTouchValue());
    if (m.isChannelPressure())  return "Channel pressure " + juce::String(m.getChannelPressureValue());
    if (m.isAllNotesOff())      return "All notes off";
    if (m.isAllSoundOff())      return "All sound off";
    if (m.isMetaEvent())        return "Meta event";

    if (m.isController())
    {
        juce::String name(juce::MidiMessage::getControllerName(m.getControllerNumber()));

        if (name.isEmpty())
            name = "[" + juce::String(m.getControllerNumber()) + "]";

        return "Controller " + name + ": " + juce::String(m.getControllerValue());
    }

    return juce::String::toHexString(m.getRawData(), m.getRawDataSize());
}

juce::String getMessageInfo(const juce::MidiMessage& message)
{
    auto time = message.getTimeStamp();

    auto hours = ((int)(time / 3600.0)) % 24;
    auto minutes = ((int)(time / 60.0)) % 60;
    auto seconds = ((int)time) % 60;
    auto millis = ((int)(time * 1000.0)) % 1000;

    auto timecode = juce::String::formatted("%02d:%02d:%02d.%03d",
        hours,
        minutes,
        seconds,
        millis);


    return(timecode + "  -  " + getMidiMessageDescription(message));
}









//==============================================================================

Seq_v3AudioProcessor::Seq_v3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())

#endif
{
}

Seq_v3AudioProcessor::~Seq_v3AudioProcessor()
{
}

//==============================================================================
const juce::String Seq_v3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Seq_v3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Seq_v3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Seq_v3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Seq_v3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Seq_v3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Seq_v3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Seq_v3AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Seq_v3AudioProcessor::getProgramName (int index)
{
    return {};
}

void Seq_v3AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================

void Seq_v3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    notes.clear();                          // [1]
    currentNote = 0;                        // [2]
    lastNoteValue = -1;                     // [3]
    time = 0;                               // [4]
    rate = static_cast<float> (sampleRate); // [5]

    euclideanRythm = EuclideanRythm(4,2);
    index = 0;


    speed = 0.05f;

    suena = true;
    velocity = 127;
    
}

void Seq_v3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Seq_v3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void Seq_v3AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // the audio buffer in a midi effect will have zero channels!
    //jassert(buffer.getNumChannels() == 0);                                                         // [6]

    //auto totalNotes = apvts.getRawParameterValue("STEPS");
    //auto events = apvts.getRawParameterValue("EVENTS");
    //DBG(totalNotes->load() << " " << events->load());

    // however we use the buffer to get timing information
    auto numSamples = buffer.getNumSamples();                                                       // [7]

    // get note duration
    auto noteDuration = static_cast<int> (std::ceil(rate * 0.25f * (0.1f + (1.0f - (speed)))));   // [8]

    auto steps = apvts.getRawParameterValue("STEPS");
    auto events = apvts.getRawParameterValue("EVENTS");
    euclideanRythm.setEuclideanRythm(steps->load(), events->load());

    midiMessages.clear();

    if ((time + numSamples) >= noteDuration)                                                        // [11]
    {
        auto offset = juce::jmax(0, juce::jmin((int)(noteDuration - time), numSamples - 1));     // [12]

        if (euclideanRythm.getEuclideanRythm()[index] == 0) {
            auto message = juce::MidiMessage::noteOff(midiChannel, noteNumber);
            midiMessages.addEvent(message, offset);
            DBG(getMessageInfo(message));
        }
        else {
            auto message = juce::MidiMessage::noteOn(midiChannel, noteNumber, (juce::uint8)velocity);
            midiMessages.addEvent(message, offset);
            DBG(getMessageInfo(message));
        }
        index = (index + 1) % euclideanRythm.getEuclideanRythm().size();

    }

    time = (time + numSamples) % noteDuration;



}

//==============================================================================

bool Seq_v3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Seq_v3AudioProcessor::createEditor()
{
    return new Seq_v3AudioProcessorEditor (*this);
}

//==============================================================================
void Seq_v3AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

}

void Seq_v3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

}

//==============================================================================

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Seq_v3AudioProcessor();
}

//==============================================================================


// Function that returns a layout of the parameters
juce::AudioProcessorValueTreeState::ParameterLayout Seq_v3AudioProcessor::createParameters() {

    // Vector with the parameters
    vector<unique_ptr<juce::RangedAudioParameter>> paramsVector;

    // We add elements to the vector
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("STEPS", "Steps", 0, 16, 4));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("EVENTS", "Events", 0, 16, 2));


    // Return the vector
    return { paramsVector.begin(), paramsVector.end() };

}

//==============================================================================



