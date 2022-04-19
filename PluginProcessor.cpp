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























EuclideanSequencerAudioProcessor::EuclideanSequencerAudioProcessor()
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

EuclideanSequencerAudioProcessor::~EuclideanSequencerAudioProcessor()
{
}

//==============================================================================
const juce::String EuclideanSequencerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EuclideanSequencerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool EuclideanSequencerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool EuclideanSequencerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double EuclideanSequencerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EuclideanSequencerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EuclideanSequencerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EuclideanSequencerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String EuclideanSequencerAudioProcessor::getProgramName(int index)
{
    return {};
}

void EuclideanSequencerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void EuclideanSequencerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{

    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    rate = static_cast<float> (sampleRate);
    bpm = getBPM();
    initNotesOnMap();

    DBG("FIN DEL PREPARE TO PLAY ");

}

void EuclideanSequencerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EuclideanSequencerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void EuclideanSequencerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    // however we use the buffer to get timing information
    numSamples = buffer.getNumSamples();
    bpm = getBPM();

    for (auto itr = euclideanRhythms.begin(); itr != euclideanRhythms.end(); ++itr) {
        // midiBuffer, puntero a EuclideanSequencerComponent, ID del objeto
        processSequencer(midiMessages, itr->second, itr->first);
    }

    //processSequencer(midiMessages, euclideanRhythms.at(0), 0);
}

//==============================================================================
bool EuclideanSequencerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EuclideanSequencerAudioProcessor::createEditor()
{
    return new EuclideanSequencerAudioProcessorEditor(*this);
}

//==============================================================================
void EuclideanSequencerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void EuclideanSequencerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    // TODO  ! ! ! ! ! ! !
    // https://docs.juce.com/master/tutorial_audio_processor_value_tree_state.html
    // https://docs.juce.com/master/tutorial_audio_parameter.html

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EuclideanSequencerAudioProcessor();
}


//==============================================================================
//==============================================================================
//==============================================================================

// Function that returns a layout of the parameters
juce::AudioProcessorValueTreeState::ParameterLayout EuclideanSequencerAudioProcessor::createParameters() {

    // Vector with the parameters
    vector<unique_ptr<juce::RangedAudioParameter>> paramsVector;

    // We add elements to the vector
    for (int i = 0; i < NUM_TOTAL_ETAPAS; i++) {

        paramsVector.push_back(make_unique<juce::AudioParameterBool>("ON_OFF_BUTTON" + to_string(i), "onOff Button " + to_string(i), false));
        paramsVector.push_back(make_unique<juce::AudioParameterInt>("STEPS" + to_string(i), "Steps " + to_string(i), 1, 32, 8));
        paramsVector.push_back(make_unique<juce::AudioParameterInt>("EVENTS" + to_string(i), "Events " + to_string(i), 0, 32, 4));
        paramsVector.push_back(make_unique<juce::AudioParameterInt>("ROTATION" + to_string(i), "Rotation " + to_string(i), 0, 32, 0));
        paramsVector.push_back(make_unique<juce::AudioParameterInt>("VELOCITY" + to_string(i), "Velocity " + to_string(i), 0, 127, 127));
        paramsVector.push_back(make_unique<juce::AudioParameterInt>("GATE" + to_string(i), "Gate " + to_string(i), 0, 400, 100));
        paramsVector.push_back(make_unique<juce::AudioParameterBool>("REVERSE_BUTTON" + to_string(i), "Reverse Button " + to_string(i), false));
        paramsVector.push_back(make_unique<juce::AudioParameterBool>("PING_PONG_BUTTON" + to_string(i), "Ping Pong Button " + to_string(i), false));
        paramsVector.push_back(make_unique<juce::AudioParameterBool>("TRIPLETS_BUTTON" + to_string(i), "Triplets Button " + to_string(i), false));
        paramsVector.push_back(make_unique<juce::AudioParameterBool>("DOTTED_BUTTON" + to_string(i), "Dotted Button " + to_string(i), false));
        paramsVector.push_back(make_unique<juce::AudioParameterChoice>("NOTE_NUMBER_COMBOBOX" + to_string(i),"Note number " + to_string(i),
            juce::StringArray("C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
                "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1",
                "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2",
                "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3",
                "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4",
                "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5",
                "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6"), 48));
        paramsVector.push_back(make_unique<juce::AudioParameterChoice>("STEP_DURATION_COMBOBOX" + to_string(i), "Step duration " + to_string(i),
            juce::StringArray("1", "1/2", "1/4","1/8", "1/16", "1/32", "1/64"), 2));
    }

    // Return the vector
    return { paramsVector.begin(), paramsVector.end() };

}

void EuclideanSequencerAudioProcessor::setNewNoteNumber(int value, int seqID) {
    if (euclideanRhythms.count(seqID))
        euclideanRhythms.at(seqID)->set_noteNumber(value);
}

void EuclideanSequencerAudioProcessor::setNewStepFigure(float duration, int seqID) {
    if (euclideanRhythms.count(seqID))
        euclideanRhythms.at(seqID)->set_figureStep(duration);
}

void EuclideanSequencerAudioProcessor::createRythm(int id, int steps, int events, int rotation,
    int velocity, int gate, int noteNumber, float figureStep, bool directionParam, bool reverseParam,
    bool pingPongParam, bool dottedNotesParam, bool tripletsParam) {

    euclideanRhythms.insert({
        id,
        new EuclideanRhythm(rate, bpm, steps, events, rotation, velocity, gate, noteNumber,
        figureStep, directionParam, reverseParam, pingPongParam, dottedNotesParam, tripletsParam)
        });
}

void EuclideanSequencerAudioProcessor::deleteRythm(int id) {
    euclideanRhythms.erase(id);
}

void EuclideanSequencerAudioProcessor::setReverseDirection(int seqID, bool reverse) {
    if (euclideanRhythms.count(seqID)) {
        euclideanRhythms.at(seqID)->set_reverse(reverse);
        // si reverse = false => direccion es false ( hacia la izq <- )
        if (reverse)
            euclideanRhythms.at(seqID)->set_direction(false);
        else
            euclideanRhythms.at(seqID)->set_direction(true);
    }
}

void EuclideanSequencerAudioProcessor::setNewPingPong(int id, bool value) {
    if (euclideanRhythms.count(id)) {
        euclideanRhythms.at(id)->set_pingPong(value);
        // si pingPong = false => devolvemos la direccion a true ( hacia la derecha -> )
        if(value = false)
            euclideanRhythms.at(id)->set_direction(true);
    }
}

void EuclideanSequencerAudioProcessor::synchronizeAll() {
    for (auto itr = euclideanRhythms.begin(); itr != euclideanRhythms.end(); itr++) {
        itr->second->convertBPMToTime();
        itr->second->setTimeStep(0);
        itr->second->setTimeNote(0);
        itr->second->setIndex(0);
        itr->second->setNumSamplesPerBar(0);
        itr->second->setCurrentSampleInBar(0);
    }
}

void EuclideanSequencerAudioProcessor::setDottedNotes(int seqID, bool value, float figureStep) {
    if (euclideanRhythms.count(seqID)) {
        euclideanRhythms.at(seqID)->set_triplets(value);
        float newDuration;
        if (value)
            newDuration = figureStep * (3.0f / 2.0f);
        else
            newDuration = figureStep;
        
        euclideanRhythms.at(seqID)->set_figureStep(newDuration);
    }
}

void EuclideanSequencerAudioProcessor::setTriplets(int seqID, bool value, float figureStep) {
    if (euclideanRhythms.count(seqID)) {
        euclideanRhythms.at(seqID)->set_dottedNotes(value);
        float newDuration;
        if (value)
            newDuration = figureStep * (2.0f / 3.0f);
        else
            newDuration = figureStep;

        euclideanRhythms.at(seqID)->set_figureStep(newDuration);

    }
}

map<int, EuclideanRhythm*> EuclideanSequencerAudioProcessor::getEuclideanRhythms() {
    return euclideanRhythms;
}


//==============================================================================

void EuclideanSequencerAudioProcessor::initNotesOnMap() {
    for (int i = 24; i <= 107; i++)
        notesOn.insert({ i, 0 });
}

int EuclideanSequencerAudioProcessor::getBPM() {

    int bpm_aux;

    // para probar en visual comentar esto
    playHead = this->getPlayHead();
    playHead->getCurrentPosition(currentPositionInfo);
    bpm_aux = currentPositionInfo.bpm;

    // para probar en ableton (DAW que sea) comentar esto
    //bpm_aux = 120;

    return bpm_aux;
}


// funcion que refresca el valor de currentSample 
int EuclideanSequencerAudioProcessor::getCurrentSampleUpdated(int numSamplesPerBar, int newNumSamplesPerBar, int currentSampleInBar) {
    return ((float)newNumSamplesPerBar / (float)numSamplesPerBar) * (float)currentSampleInBar;
}

int EuclideanSequencerAudioProcessor::getIndexFromCurrentSample(EuclideanRhythm* e) {
    if (e->get_steps() == 0)
        return 0;
    int aux;
    if (e->get_direction() == false)
        aux = ceil((float)e->getCurrentSampleInBar() / (float)e->getStepDuration());
    else
        aux = floor((float)e->getCurrentSampleInBar() / (float)e->getStepDuration());
    return aux % e->get_steps();
}

//==============================================================================

void EuclideanSequencerAudioProcessor::processSequencer(juce::MidiBuffer& midiMessages, EuclideanRhythm* euclideanRhythm, int ID) {



    int gate = *apvts.getRawParameterValue("GATE" + to_string(ID));
    euclideanRhythm->set_gate(gate);

    euclideanRhythm->setBpm(getBPM());
    euclideanRhythm->setRate(rate);
    euclideanRhythm->convertBPMToTime();

    // actualizamos todo el rato el n�mero de samples totales del comp�s 
    // para tener la aguja actualizada
    euclideanRhythm->setNumSamplesPerBar(euclideanRhythm->get_steps() * euclideanRhythm->getStepDuration());

    int steps = *apvts.getRawParameterValue("STEPS" + to_string(ID));
    int events = *apvts.getRawParameterValue("EVENTS" + to_string(ID));
    int newRotation = *apvts.getRawParameterValue("ROTATION" + to_string(ID));

    my_mutex[ID].lock();
    euclideanRhythm->set_euclideanRhythm(steps, events);
    my_mutex[ID].unlock();


    // check if it has to rotate, and if it has to do it to the right or to the left
    if (newRotation > euclideanRhythm->get_rotation()) {
        euclideanRhythm->rotateRight(newRotation - euclideanRhythm->get_rotation());
        euclideanRhythm->set_rotation(newRotation);
    }
    else if (newRotation < euclideanRhythm->get_rotation()) {
        euclideanRhythm->rotateLeft(euclideanRhythm->get_rotation() - newRotation);
        euclideanRhythm->set_rotation(newRotation);
    }


    // cs1/ts1 = cs2/ts2 donde cs1 y ts1 son el current y el total samples antes de cambiar los steps y
    // cs2 y ts2 son el current y el total samples despues de cambiarlos  
    euclideanRhythm->setCurrentSampleInBar(getCurrentSampleUpdated(euclideanRhythm->getNumSamplesPerBar(),
        euclideanRhythm->getStepDuration() * euclideanRhythm->get_steps(),
        euclideanRhythm->getCurrentSampleInBar()));


    // a partir del current sample que estamos procesando, sacamos el index del ritmo
    euclideanRhythm->setIndex(getIndexFromCurrentSample(euclideanRhythm));

    // comprobamos el tiempo que lleva durando cada nota
    // si ha excedido >= noteDuration se manda un noteOff de esa nota
    // en caso contrario se actualiza el tiempo que lleva sonando
    for (auto itr = euclideanRhythm->notesDurationMap.begin(); itr != euclideanRhythm->notesDurationMap.end(); ++itr) {
        if (itr->second + numSamples >= euclideanRhythm->getNoteDuration()) {
            auto offset = juce::jmax(0, juce::jmin((int)(euclideanRhythm->getNoteDuration() - itr->second), numSamples - 1));
            auto message = juce::MidiMessage::noteOff(midiChannel, itr->first);


            DBG(" seqID: " << ID << " " <<
                getMidiMessageDescription(message) << " noteNumber " << itr->first <<
                " figureStep " << euclideanRhythm->get_figureStep() <<
                " index " << euclideanRhythm->getIndex() <<
                " on (" << steps << "," << events << ") " <<
                " rotation " << euclideanRhythm->get_rotation() <<
                " on " << euclideanRhythm->getList() <<
                " direction " << (int)euclideanRhythm->get_direction() <<
                " currentSample " << euclideanRhythm->getCurrentSampleInBar() <<
                " of " << euclideanRhythm->getNumSamplesPerBar());

            // mapa de notas global
            notesOn[itr->first]--;
            if (notesOn[itr->first] == 0)
                midiMessages.addEvent(message, offset);

            // vector para borrar notas del mapa local
            euclideanRhythm->notesToDeleteFromMap.push_back(itr->first);
        }
        else {
            itr->second = (itr->second + numSamples);
        }
    }

    // vector auxiliar para borrar las notas del mapa que han dejado de sonar
    for (auto itr = euclideanRhythm->notesToDeleteFromMap.begin(); itr != euclideanRhythm->notesToDeleteFromMap.end(); ++itr) {
        euclideanRhythm->notesDurationMap.erase(*itr);
    }
    euclideanRhythm->notesToDeleteFromMap.clear();

    // If de si debe empezar un step o no 
    if ((euclideanRhythm->getTimeStep() + numSamples) >= euclideanRhythm->getStepDuration()) {

        auto offset = juce::jmax(0, juce::jmin((int)(euclideanRhythm->getStepDuration() - euclideanRhythm->getTimeStep()), numSamples - 1));

        // empieza a sonar una nota lanzando el noteOn
        // anyadimos 
        if (euclideanRhythm->get_euclideanRhythm().at(euclideanRhythm->getIndex()) == 1) {
            int note = euclideanRhythm->get_noteNumber();
            int velocity = *apvts.getRawParameterValue("VELOCITY" + to_string(ID));
            euclideanRhythm->set_velocity(velocity);

            auto message = juce::MidiMessage::noteOn(midiChannel, note, (juce::uint8)velocity);
            DBG(" seqID: " << ID << " " <<
                getMidiMessageDescription(message) << " noteNumber " << note <<
                " figureStep " << euclideanRhythm->get_figureStep() <<
                " index " << euclideanRhythm->getIndex() <<
                " on (" << steps << "," << events << ") " <<
                " rotation " << euclideanRhythm->get_rotation() <<
                " velocity " << velocity <<
                " on " << euclideanRhythm->getList() <<
                " direction " << (int)euclideanRhythm->get_direction() <<
                " currentSample " << euclideanRhythm->getCurrentSampleInBar() <<
                " of " << euclideanRhythm->getNumSamplesPerBar());

            midiMessages.addEvent(message, offset);

            notesOn[note]++;
            euclideanRhythm->notesDurationMap.insert({ note, numSamples - offset });
        }
    }

    // comprobamos el sentido en el que estamos procesando
    if(!euclideanRhythm->get_reverse() && !euclideanRhythm->get_pingPong())
        euclideanRhythm->set_direction(true);
    
    // actualizamos el numero de sample que acabamos de procesar dependiendo del sentido
    int newCurrentSampleInBar;
    // direccion horaria
    if (euclideanRhythm->get_direction()) {
        newCurrentSampleInBar = (euclideanRhythm->getCurrentSampleInBar() + numSamples) %
            (euclideanRhythm->getStepDuration() * euclideanRhythm->get_steps());
        // ping-pong mode on
        if (euclideanRhythm->get_pingPong()) {
            if (euclideanRhythm->getCurrentSampleInBar() + numSamples >= euclideanRhythm->getNumSamplesPerBar()) {
                newCurrentSampleInBar = euclideanRhythm->getNumSamplesPerBar();
                euclideanRhythm->switchDirection();
            }
        }
    }
    // direccion anti horaria
    else {
        if (euclideanRhythm->getCurrentSampleInBar() - numSamples >= 0)
            newCurrentSampleInBar = euclideanRhythm->getCurrentSampleInBar() - numSamples;
        else
            newCurrentSampleInBar = (euclideanRhythm->getNumSamplesPerBar() -
                (numSamples - euclideanRhythm->getCurrentSampleInBar()));
        // ping-pong mode on
        if (euclideanRhythm->get_pingPong()) {
            if (euclideanRhythm->getCurrentSampleInBar() - numSamples <= 0) {
                newCurrentSampleInBar = 0;
                euclideanRhythm->switchDirection();
            }
        }
    }
    euclideanRhythm->setCurrentSampleInBar(newCurrentSampleInBar);

    // actualizamos el tiempo que lleva sonando la nota
    euclideanRhythm->setTimeStep((euclideanRhythm->getTimeStep() + numSamples) %
        euclideanRhythm->getStepDuration());

}












