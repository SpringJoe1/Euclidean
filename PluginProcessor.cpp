/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
















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
    rate = static_cast<float> (sampleRate);
    bpm = getBPM();


    // TODO -- poner sliders en cada seq de la velocity
    velocity = 127;


    numTotalSequencers = 0;
    euclideanRythms.insert({ 0, new EuclideanRythmComponent(8, 4, 0) });

    // negra = 1
    euclideanRythms.at(0)->setFigureStep(1);
    euclideanRythms.at(0)->setFigureNote(1);
    convertBPMToTime(euclideanRythms.at(0));



    euclideanRythms.at(0)->setTimeStep(euclideanRythms.at(0)->getStepDuration());
    euclideanRythms.at(0)->setTimeNote(0);


    euclideanRythms.at(0)->setIndex(0);

    euclideanRythms.at(0)->setRotationValue(0);

    euclideanRythms.at(0)->setNoteNumber(72);   // C4


    euclideanRythms.at(0)->setNumSamplesPerBar(0);

    euclideanRythms.at(0)->setCurrentSampleInBar(0);

    DBG("FIN DEL PREPARE TO PLAY ");

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
    numSamples = buffer.getNumSamples();

    bpm = getBPM();
    for (auto itr = euclideanRythms.begin(); itr != euclideanRythms.end(); ++itr) {
        // midiBuffer, puntero a EuclideanSequencerComponent, ID del objeto
        processSequencer(midiMessages, itr->second, itr->first);
    }
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
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("STEPS0", "Steps", 0, 16, 8));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("EVENTS0", "Events", 0, 16, 4));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("ROTATION0", "Rotation", 0, 16, 0));

    // C4 by default (index 48 in the StringArray)
    //paramsVector.push_back(make_unique<juce::AudioParameterChoice>("NOTE_NUMBER", "Note",
    //    juce::StringArray("C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
    //        "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
    //        "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
    //        "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
    //        "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
    //        "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
    //        "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6"), 48));


    // Return the vector
    return { paramsVector.begin(), paramsVector.end() };

}

void Seq_v4AudioProcessor::setNewNoteNumber(int value, int seqID) {
    DBG("notevalue " << value << " seqID " << seqID);
    if (seqID >= 0 && seqID < NUM_TOTAL_ETAPAS) {
        euclideanRythms.at(seqID)->setNoteNumber(value);
    }

}

void Seq_v4AudioProcessor::setNewNoteDuration(float duration, int seqID) {
    if (seqID >= 0 && seqID < NUM_TOTAL_ETAPAS) {
        euclideanRythms.at(seqID)->setFigureNote(duration);
    }
}
void Seq_v4AudioProcessor::setNewStepDuration(float duration, int seqID) {
    if (seqID >= 0 && seqID < NUM_TOTAL_ETAPAS) {
        euclideanRythms.at(seqID)->setFigureStep(duration);
    }
}
//==============================================================================

int Seq_v4AudioProcessor::getBPM() {
    
    int bpm_aux;
    
    // para probar en visual comentar esto
    /*playHead = this->getPlayHead();
    playHead->getCurrentPosition(currentPositionInfo);
    bpm_aux = currentPositionInfo.bpm;*/

    // para probar en ableton (DAW que sea) comentar esto
    bpm_aux = 120;

    return bpm_aux;
}

// Funciones auxiliares
//esta funcion saca el tiempo en funcion de los bpms
void Seq_v4AudioProcessor::convertBPMToTime(EuclideanRythmComponent* e) {

    e->setNoteDuration(round((((float)(60000 * rate * e->getFigureNote()) / bpm) / 1000) * 100) / 100);
    e->setStepDuration(round((((float)(60000 * rate * e->getFigureStep()) / bpm) / 1000) * 100) / 100);

}

// funcion que refresca el valor de currentSample 
int Seq_v4AudioProcessor::getCurrentSampleUpdated(int numSamplesPerBar, int newNumSamplesPerBar, int currentSampleInBar) {
    return ((float)newNumSamplesPerBar / (float)numSamplesPerBar) * (float)currentSampleInBar;
}

int Seq_v4AudioProcessor::getIndexFromCurrentSample(EuclideanRythmComponent* e) {
    int aux = ceil((float)e->getCurrentSampleInBar() / (float)e->getStepDuration());
    return aux % e->get_steps();
}


//==============================================================================

void Seq_v4AudioProcessor::processSequencer(juce::MidiBuffer& midiMessages, EuclideanRythmComponent* euclideanRythm, int ID) {

    convertBPMToTime(euclideanRythm);

    // actualizamos todo el rato el n�mero de samples totales del comp�s 
    // para tener la aguja actualizada
    euclideanRythm->setNumSamplesPerBar(euclideanRythm->get_steps() * euclideanRythm->getStepDuration());

    int steps = *apvts.getRawParameterValue("STEPS" + to_string(ID));
    int events = *apvts.getRawParameterValue("EVENTS" + to_string(ID));
    int newRotation = *apvts.getRawParameterValue("ROTATION" + to_string(ID));
    
    euclideanRythm->set_euclideanRythm(steps, events, euclideanRythm->getRotationValue());


    // check if it has to rotate, and if it has to do it to the right or to the left
    if (newRotation > euclideanRythm->getRotationValue()) {
        DBG("antes rotation = " << euclideanRythm->getRotationValue() << " on " << euclideanRythm->getList());
        euclideanRythm->rotateRight(newRotation - euclideanRythm->getRotationValue());
        euclideanRythm->setRotationValue(newRotation);
        DBG("despues rotation = " << euclideanRythm->getRotationValue() << " on " << euclideanRythm->getList());

    }
    else if (newRotation < euclideanRythm->getRotationValue()) {
        DBG("antes rotation = " << euclideanRythm->getRotationValue() << " on " << euclideanRythm->getList());
        euclideanRythm->rotateLeft(euclideanRythm->getRotationValue() - newRotation);
        euclideanRythm->setRotationValue(newRotation);
        DBG("despues rotation = " << euclideanRythm->getRotationValue() << " on " << euclideanRythm->getList());
    }


    // cs1/ts1 = cs2/ts2 donde cs1 y ts1 son el current y el total samples antes de cambiar los steps y
    // cs2 y ts2 son el current y el total samples despues de cambiarlos  
    euclideanRythm->setCurrentSampleInBar(getCurrentSampleUpdated(euclideanRythm->getNumSamplesPerBar(),
        euclideanRythm->getStepDuration() * euclideanRythm->get_steps(),
        euclideanRythm->getCurrentSampleInBar()));
    
    // a partir del current sample que estamos procesando, sacamos el index del ritmo
    euclideanRythm->setIndex(getIndexFromCurrentSample(euclideanRythm));

    // TODO -- revisar ESTO DEBO HACERLO ?
    midiMessages.clear();

    // comprobamos el tiempo que lleva durando cada nota
    // si ha excedido >= noteDuration se manda un noteOff de esa nota
    // en caso contrario se actualiza el tiempo que lleva sonando
    for (auto itr = euclideanRythm->notesDurationMap.begin(); itr != euclideanRythm->notesDurationMap.end(); ++itr) {
        if (itr->second + numSamples >= euclideanRythm->getNoteDuration()) {
            auto offset = juce::jmax(0, juce::jmin((int)(euclideanRythm->getNoteDuration() - itr->second), numSamples - 1));
            auto message = juce::MidiMessage::noteOff(midiChannel, itr->first);
            midiMessages.addEvent(message, offset);
            euclideanRythm->notesToDeleteFromMap.push_back(itr->first);
        }
        else {
            itr->second = (itr->second + numSamples) % euclideanRythm->getNoteDuration();
        }
    }

    // vector auxiliar para borrar las notas del mapa que han dejado de sonar
    for (auto itr = euclideanRythm->notesToDeleteFromMap.begin(); itr != euclideanRythm->notesToDeleteFromMap.end(); ++itr) {
        euclideanRythm->notesDurationMap.erase(*itr);
    }
    euclideanRythm->notesToDeleteFromMap.clear();

    if ((euclideanRythm->getTimeStep() + numSamples) >= euclideanRythm->getStepDuration()) {

        auto offset = juce::jmax(0, juce::jmin((int)(euclideanRythm->getStepDuration() - euclideanRythm->getTimeStep()), numSamples - 1));

        // empieza a sonar una nota lanzando el noteOn
        // a�adimos 
        if (euclideanRythm->get_euclideanRythm().at(euclideanRythm->getIndex()) == 1) {
            int note = euclideanRythm->getNoteNumber();
            auto message = juce::MidiMessage::noteOn(midiChannel, note, (juce::uint8)velocity);
            DBG(getMessageInfo(message) << " index " << euclideanRythm->getIndex() << " steps " << steps << " MIDInote " << note);
            midiMessages.addEvent(message, offset);
            euclideanRythm->notesDurationMap.insert({ note, numSamples - offset });
        }
    }


    // actualizamos el numero de sample que acabamos de procesar
    euclideanRythm->setCurrentSampleInBar((euclideanRythm->getCurrentSampleInBar() + numSamples) %
        (euclideanRythm->get_steps() * euclideanRythm->getStepDuration()));//((int)steps->load()*stepDuration);

    //TODO cerrar notas en funcino de noteDuration
    
    euclideanRythm->setTimeStep((euclideanRythm->getTimeStep() + numSamples) %
        euclideanRythm->getStepDuration());

}












