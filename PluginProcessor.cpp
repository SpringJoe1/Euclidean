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
Seq_v4AudioProcessor::Seq_v4AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ), apvts(*this, nullptr, "Parameters", createParameters())

#endif
{
}

Seq_v4AudioProcessor::~Seq_v4AudioProcessor()
{
}

//==============================================================================
const juce::String Seq_v4AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Seq_v4AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Seq_v4AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Seq_v4AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double Seq_v4AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Seq_v4AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Seq_v4AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Seq_v4AudioProcessor::setCurrentProgram(int index)
{
}

const juce::String Seq_v4AudioProcessor::getProgramName(int index)
{
    return {};
}

void Seq_v4AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void Seq_v4AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    guarrada1 = true;

    //TODO de momento tanto los stpes como las notas son negras
    figureStep = 1;
    figureNote = 1;
    rate = static_cast<float> (sampleRate);

    convertBPMToTime();


    timeStep = stepDuration;
    timeNote = 0;



    velocity = 127;

    // TODO cambiar el 4
    euclideanRythm = EuclideanRythm(8, 4, 0);
    index = 0;
    rotationValue = 0;
    currentNoteNumber = 72;  // C4
    newNoteNumber = 72;  // C4

    numSamplesPerBar = 0;
    currentSampleInBar = 0;
}

void Seq_v4AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Seq_v4AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
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

void Seq_v4AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // however we use the buffer to get timing information
    auto numSamples = buffer.getNumSamples();

    convertBPMToTime();

    // actualizamos todo el rato el número de samples totales del compás 
    // para tener la aguja actualizada
    numSamplesPerBar = euclideanRythm.getSteps() * stepDuration;

    int steps = *apvts.getRawParameterValue("STEPS");
    int events = *apvts.getRawParameterValue("EVENTS");
    int newRotation = *apvts.getRawParameterValue("ROTATION");
    
    euclideanRythm.setEuclideanRythm(steps, events, rotationValue);

    // check if it has to rotate, and if it has to do it to the right or to the left
    if (newRotation > rotationValue) {
        DBG("antes rotation = " << rotationValue << " on " << euclideanRythm.getList());
        euclideanRythm.rotateRight(newRotation - rotationValue);
        rotationValue = newRotation;
        DBG("despues rotation = " << rotationValue << " on " << euclideanRythm.getList());

    }
    else if (newRotation < rotationValue) {
        euclideanRythm.rotateLeft(rotationValue - newRotation);
        rotationValue = newRotation;
    }


    // cs1/ts1 = cs2/ts2 donde cs1 y ts1 son el current y el total samples antes de cambiar los steps y
    // cs2 y ts2 son el current y el total samples despues de cambiarlos
    currentSampleInBar = getCurrentSampleUpdated(numSamplesPerBar, stepDuration * euclideanRythm.getSteps());
    // a partir del current sample que estamos procesando, sacamos el index del ritmo
    index = getIndexFromCurrentSample();

    midiMessages.clear();

    //TODO cambiar a stepduration
    if ((timeStep + numSamples) >= stepDuration)
    {
        auto offset = juce::jmax(0, juce::jmin((int)(stepDuration - timeStep), numSamples - 1));
            

        if (currentNoteNumber != newNoteNumber) {
            // hacemos que la nota actual deje de sonar 
            auto message = juce::MidiMessage::noteOff(midiChannel, currentNoteNumber);
            midiMessages.addEvent(message, offset);
            DBG("cambio de nota " << currentNoteNumber << " a " << newNoteNumber);
            DBG(getMessageInfo(message) << " index " << index << " steps " << steps << " MIDInote " << currentNoteNumber);
            // actualizamos la nota
            updateNoteNumber();
        }

        if (euclideanRythm.getEuclideanRythm()[index] == 1) {
            auto message = juce::MidiMessage::noteOn(midiChannel, newNoteNumber, (juce::uint8)velocity);
            midiMessages.addEvent(message, offset);
            //DBG("rotation = " << rotationValue << " on " << euclideanRythm.getList());
            DBG(getMessageInfo(message) << " index " << index << " steps " << steps << " MIDInote " << currentNoteNumber);
        }
        else {
            auto message = juce::MidiMessage::noteOff(midiChannel, currentNoteNumber);
            midiMessages.addEvent(message, offset);
            //DBG("rotation = " << rotationValue << " on " << euclideanRythm.getList());
            DBG(getMessageInfo(message) << " index " << index << " steps " << steps << " MIDInote " << currentNoteNumber);
        }
        // TODO no se donde poner esta vaina ! ! ! ! ! ! ! 
    }

    // actualizamos el numero de sample que acabamos de procesar
    currentSampleInBar = (currentSampleInBar + numSamples) % (euclideanRythm.getSteps() * stepDuration);//((int)steps->load()*stepDuration);

    //TODO cerrar notas en funcino de noteDuration
    timeStep = (timeStep + numSamples) % stepDuration;
    timeNote = (timeNote + numSamples) % noteDuration;


}

//==============================================================================
bool Seq_v4AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Seq_v4AudioProcessor::createEditor()
{
    return new Seq_v4AudioProcessorEditor(*this);
}

//==============================================================================
void Seq_v4AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Seq_v4AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Seq_v4AudioProcessor();
}


//==============================================================================
//==============================================================================
//==============================================================================

// Function that returns a layout of the parameters
juce::AudioProcessorValueTreeState::ParameterLayout Seq_v4AudioProcessor::createParameters() {

    // Vector with the parameters
    vector<unique_ptr<juce::RangedAudioParameter>> paramsVector;

    // We add elements to the vector
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("STEPS", "Steps", 0, 16, 8));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("EVENTS", "Events", 0, 16, 4));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("ROTATION", "Rotation", 0, 16, 0));
    // C4 by default (index 48 in the StringArray)
    paramsVector.push_back(make_unique<juce::AudioParameterChoice>("NOTE_NUMBER", "Note",
        juce::StringArray ("C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
        "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
        "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
        "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
        "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
        "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
        "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6"), 48));


    // Return the vector
    return { paramsVector.begin(), paramsVector.end() };

}

void Seq_v4AudioProcessor::setNewNoteNumber(int note) {
    newNoteNumber = note;
}


void Seq_v4AudioProcessor::updateNoteNumber() {
    currentNoteNumber = newNoteNumber;
}


//==============================================================================

// Funciones auxiliares
//esta funcion saca el tiempo en funcion de los bpms
void Seq_v4AudioProcessor::convertBPMToTime() {

    // para probar en visual comentar esto
    playHead = this->getPlayHead();
    playHead->getCurrentPosition(currentPositionInfo);
    int bpm = currentPositionInfo.bpm;

    // para probar en ableton (DAW que sea) comentar esto
    //int bpm = 120;


    //FIXME cambiar cmath por la biblioteca de matematica de juce
    noteDuration = round((((60000 * rate * figureNote) / bpm) / 1000) * 100) / 100;
    stepDuration = round((((60000 * rate * figureStep) / bpm) / 1000) * 100) / 100;
}


// funcion que refresca el valor de currentSample 
int Seq_v4AudioProcessor::getCurrentSampleUpdated(int numSamplesPerBar, int newNumSamplesPerBar) {
    return ((float)newNumSamplesPerBar / (float)numSamplesPerBar) * (float)currentSampleInBar;
}

int Seq_v4AudioProcessor::getIndexFromCurrentSample() {
    int aux = ceil((float)currentSampleInBar / (float)stepDuration);
    return aux % euclideanRythm.getSteps();
}

EuclideanRythm Seq_v4AudioProcessor::getEuclideanRythm() {
    return euclideanRythm;
}
//==============================================================================

