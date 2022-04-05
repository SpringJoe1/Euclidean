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























EucSeq_MultiStageAudioProcessor::EucSeq_MultiStageAudioProcessor()
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

EucSeq_MultiStageAudioProcessor::~EucSeq_MultiStageAudioProcessor()
{
}

//==============================================================================
const juce::String EucSeq_MultiStageAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool EucSeq_MultiStageAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool EucSeq_MultiStageAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool EucSeq_MultiStageAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double EucSeq_MultiStageAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int EucSeq_MultiStageAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int EucSeq_MultiStageAudioProcessor::getCurrentProgram()
{
    return 0;
}

void EucSeq_MultiStageAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String EucSeq_MultiStageAudioProcessor::getProgramName(int index)
{
    return {};
}

void EucSeq_MultiStageAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void EucSeq_MultiStageAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    rate = static_cast<float> (sampleRate);
    bpm = getBPM();
    initNotesOnMap();

    // TODO -- poner sliders en cada seq de la velocity
    velocity = 127;

    //numTotalSequencers = 0;
    
    //==============================================================================
    // Seq 1
    //==============================================================================

    euclideanRhythms.insert({ 0, new EuclideanRhythmComponent(8, 4) });
    euclideanRhythms.at(0)->set_rotation(0);



    // negra = 1
    euclideanRhythms.at(0)->setFigureStep(1);
    euclideanRhythms.at(0)->setFigureNote(1);
    convertBPMToTime(euclideanRhythms.at(0));



    euclideanRhythms.at(0)->setTimeStep(euclideanRhythms.at(0)->getStepDuration());
    euclideanRhythms.at(0)->setTimeNote(0);


    euclideanRhythms.at(0)->setIndex(0);

    //euclideanRhythms.at(0)->setRotationValue(0);

    euclideanRhythms.at(0)->setNoteNumber(72);   // C4


    euclideanRhythms.at(0)->setNumSamplesPerBar(0);

    euclideanRhythms.at(0)->setCurrentSampleInBar(0);

    //==============================================================================
    // Seq 2
    //==============================================================================

    euclideanRhythms.insert({ 1, new EuclideanRhythmComponent(8, 4) });
    euclideanRhythms.at(1)->set_rotation(0);



    // negra = 1
    euclideanRhythms.at(1)->setFigureStep(1);
    euclideanRhythms.at(1)->setFigureNote(1);
    convertBPMToTime(euclideanRhythms.at(1));



    euclideanRhythms.at(1)->setTimeStep(euclideanRhythms.at(1)->getStepDuration());
    euclideanRhythms.at(1)->setTimeNote(0);


    euclideanRhythms.at(1)->setIndex(0);

    //euclideanRhythms.at(0)->setRotationValue(0);

    euclideanRhythms.at(1)->setNoteNumber(72);   // C4


    euclideanRhythms.at(1)->setNumSamplesPerBar(0);

    euclideanRhythms.at(1)->setCurrentSampleInBar(0);

    //==============================================================================
    // Seq 3
    //==============================================================================

    euclideanRhythms.insert({ 2, new EuclideanRhythmComponent(8, 4) });
    euclideanRhythms.at(2)->set_rotation(0);



    // negra = 1
    euclideanRhythms.at(2)->setFigureStep(1);
    euclideanRhythms.at(2)->setFigureNote(1);
    convertBPMToTime(euclideanRhythms.at(2));



    euclideanRhythms.at(2)->setTimeStep(euclideanRhythms.at(2)->getStepDuration());
    euclideanRhythms.at(2)->setTimeNote(0);


    euclideanRhythms.at(2)->setIndex(0);

    //euclideanRhythms.at(0)->setRotationValue(0);

    euclideanRhythms.at(2)->setNoteNumber(72);   // C4


    euclideanRhythms.at(2)->setNumSamplesPerBar(0);

    euclideanRhythms.at(2)->setCurrentSampleInBar(0);

    DBG("FIN DEL PREPARE TO PLAY ");

}

void EucSeq_MultiStageAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool EucSeq_MultiStageAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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

void EucSeq_MultiStageAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{

    // however we use the buffer to get timing information
    numSamples = buffer.getNumSamples();
    bpm = getBPM();

    //for (auto itr = euclideanRhythms.begin(); itr != euclideanRhythms.end(); ++itr) {
    //    // midiBuffer, puntero a EuclideanSequencerComponent, ID del objeto
    //    processSequencer(midiMessages, itr->second, itr->first);
    //}
    processSequencer(midiMessages, euclideanRhythms.at(0), 0);
}

//==============================================================================
bool EucSeq_MultiStageAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* EucSeq_MultiStageAudioProcessor::createEditor()
{
    return new EucSeq_MultiStageAudioProcessorEditor(*this);
}

//==============================================================================
void EucSeq_MultiStageAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void EucSeq_MultiStageAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new EucSeq_MultiStageAudioProcessor();
}


//==============================================================================
//==============================================================================
//==============================================================================

// Function that returns a layout of the parameters
juce::AudioProcessorValueTreeState::ParameterLayout EucSeq_MultiStageAudioProcessor::createParameters() {

    // Vector with the parameters
    vector<unique_ptr<juce::RangedAudioParameter>> paramsVector;

    // We add elements to the vector
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("STEPS0", "Steps", 0, 16, 8));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("EVENTS0", "Events", 0, 16, 4));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("ROTATION0", "Rotation", 0, 16, 0));

    paramsVector.push_back(make_unique<juce::AudioParameterInt>("STEPS1", "Steps", 0, 16, 8));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("EVENTS1", "Events", 0, 16, 4));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("ROTATION1", "Rotation", 0, 16, 0));

    paramsVector.push_back(make_unique<juce::AudioParameterInt>("STEPS2", "Steps", 0, 16, 8));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("EVENTS2", "Events", 0, 16, 4));
    paramsVector.push_back(make_unique<juce::AudioParameterInt>("ROTATION2", "Rotation", 0, 16, 0));

    // Return the vector
    return { paramsVector.begin(), paramsVector.end() };

}

void EucSeq_MultiStageAudioProcessor::setNewNoteNumber(int value, int seqID) {
    if (seqID >= 0 && seqID < NUM_TOTAL_ETAPAS) {
        euclideanRhythms.at(seqID)->setNoteNumber(value);
    }

}

void EucSeq_MultiStageAudioProcessor::setNewNoteDuration(float duration, int seqID) {
    if (seqID >= 0 && seqID < NUM_TOTAL_ETAPAS) {
        euclideanRhythms.at(seqID)->setFigureNote(duration);
    }
}
void EucSeq_MultiStageAudioProcessor::setNewStepDuration(float duration, int seqID) {
    if (seqID >= 0 && seqID < NUM_TOTAL_ETAPAS) {
        euclideanRhythms.at(seqID)->setFigureStep(duration);
    }
}
//==============================================================================

void EucSeq_MultiStageAudioProcessor::initNotesOnMap() {
    for (int i = 24; i <= 107; i++)
        notesOn.insert( { i, 0 } );
}

int EucSeq_MultiStageAudioProcessor::getBPM() {
    
    int bpm_aux;
    
    // para probar en visual comentar esto
    //playHead = this->getPlayHead();
    //playHead->getCurrentPosition(currentPositionInfo);
    //bpm_aux = currentPositionInfo.bpm;

    // para probar en ableton (DAW que sea) comentar esto
    bpm_aux = 50;

    return bpm_aux;
}

// Funciones auxiliares
//esta funcion saca el tiempo en funcion de los bpms
void EucSeq_MultiStageAudioProcessor::convertBPMToTime(EuclideanRhythmComponent* e) {

    e->setNoteDuration(round((((float)(60000 * rate * e->getFigureNote()) / bpm) / 1000) * 100) / 100);
    e->setStepDuration(round((((float)(60000 * rate * e->getFigureStep()) / bpm) / 1000) * 100) / 100);

}

// funcion que refresca el valor de currentSample 
int EucSeq_MultiStageAudioProcessor::getCurrentSampleUpdated(int numSamplesPerBar, int newNumSamplesPerBar, int currentSampleInBar) {
    return ((float)newNumSamplesPerBar / (float)numSamplesPerBar) * (float)currentSampleInBar;
}

int EucSeq_MultiStageAudioProcessor::getIndexFromCurrentSample(EuclideanRhythmComponent* e) {
    if (e->get_steps() == 0)
        return 0;
    int aux = ceil((float)e->getCurrentSampleInBar() / (float)e->getStepDuration());
    return aux % e->get_steps();
}


//==============================================================================

void EucSeq_MultiStageAudioProcessor::processSequencer(juce::MidiBuffer& midiMessages, EuclideanRhythmComponent* euclideanRhythm, int ID) {

    convertBPMToTime(euclideanRhythm);

    // actualizamos todo el rato el n�mero de samples totales del comp�s 
    // para tener la aguja actualizada
    euclideanRhythm->setNumSamplesPerBar(euclideanRhythm->get_steps() * euclideanRhythm->getStepDuration());

    int steps = *apvts.getRawParameterValue("STEPS" + to_string(ID));
    int events = *apvts.getRawParameterValue("EVENTS" + to_string(ID));
    int newRotation = *apvts.getRawParameterValue("ROTATION" + to_string(ID));
    
    DBG("antes de crear ritmo " << newRotation << " current rotation " << euclideanRhythm->get_rotation() << " " << euclideanRhythm->getList());
    euclideanRhythm->set_euclideanRhythm(steps, events);
    DBG("despues crear ritmo " << newRotation << " current rotation " << euclideanRhythm->get_rotation() << " " << euclideanRhythm->getList());

    // check if it has to rotate, and if it has to do it to the right or to the left
    if (newRotation > euclideanRhythm->get_rotation()) {
        DBG("RIGHT 1 newRotation " << newRotation << " current rotation " << euclideanRhythm->get_rotation());
        euclideanRhythm->rotateRight(newRotation - euclideanRhythm->get_rotation());
        euclideanRhythm->set_rotation(newRotation);
        DBG("RIGHT 2 newRotation " << newRotation << " current rotation " << euclideanRhythm->get_rotation());
    }
    else if (newRotation < euclideanRhythm->get_rotation()) {
        DBG("LEFT 1 newRotation " << newRotation << " current rotation " << euclideanRhythm->get_rotation());
        euclideanRhythm->rotateLeft(euclideanRhythm->get_rotation() - newRotation);
        euclideanRhythm->set_rotation(newRotation);
        DBG("LEFT 2 newRotation " << newRotation << " current rotation " << euclideanRhythm->get_rotation());
    }
    DBG("FUERA ELSE IF " << newRotation << " current rotation " << euclideanRhythm->get_rotation() << " " << euclideanRhythm->getList());


    // cs1/ts1 = cs2/ts2 donde cs1 y ts1 son el current y el total samples antes de cambiar los steps y
    // cs2 y ts2 son el current y el total samples despues de cambiarlos  
    euclideanRhythm->setCurrentSampleInBar(getCurrentSampleUpdated(euclideanRhythm->getNumSamplesPerBar(),
        euclideanRhythm->getStepDuration() * euclideanRhythm->get_steps(),
        euclideanRhythm->getCurrentSampleInBar()));
    
    // a partir del current sample que estamos procesando, sacamos el index del ritmo
    euclideanRhythm->setIndex(getIndexFromCurrentSample(euclideanRhythm));

    // TODO -- revisar ESTO DEBO HACERLO ?
    //midiMessages.clear();

    // comprobamos el tiempo que lleva durando cada nota
    // si ha excedido >= noteDuration se manda un noteOff de esa nota
    // en caso contrario se actualiza el tiempo que lleva sonando
    for (auto itr = euclideanRhythm->notesDurationMap.begin(); itr != euclideanRhythm->notesDurationMap.end(); ++itr) {
        if (itr->second + numSamples >= euclideanRhythm->getNoteDuration()) {
            auto offset = juce::jmax(0, juce::jmin((int)(euclideanRhythm->getNoteDuration() - itr->second), numSamples - 1));
            auto message = juce::MidiMessage::noteOff(midiChannel, itr->first);
            DBG(getMessageInfo(message) << " index " << euclideanRhythm->getIndex() << " steps " << steps <<
                " MIDInote " << itr->first << " rotation " << euclideanRhythm->get_rotation() << " on " << euclideanRhythm->getList());
            
            // mapa de notas global
            notesOn[itr->first]--;
            if(notesOn[itr->first] == 0)
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

    // If de si debe empezar una step o no 
    if ((euclideanRhythm->getTimeStep() + numSamples) >= euclideanRhythm->getStepDuration()) {

        auto offset = juce::jmax(0, juce::jmin((int)(euclideanRhythm->getStepDuration() - euclideanRhythm->getTimeStep()), numSamples - 1));

        // empieza a sonar una nota lanzando el noteOn
        // a�adimos 
        if (euclideanRhythm->get_euclideanRhythm().at(euclideanRhythm->getIndex()) == 1) {
            int note = euclideanRhythm->getNoteNumber();
            auto message = juce::MidiMessage::noteOn(midiChannel, note, (juce::uint8)velocity);
            DBG(getMessageInfo(message) << " index " << euclideanRhythm->getIndex() << " steps " << steps <<
                " MIDInote " << note << " rotation " << euclideanRhythm->get_rotation() << " on " << euclideanRhythm->getList());
            midiMessages.addEvent(message, offset);
            notesOn[note]++;
            euclideanRhythm->notesDurationMap.insert({ note, numSamples - offset });
        }
    }


    // actualizamos el numero de sample que acabamos de procesar
    euclideanRhythm->setCurrentSampleInBar((euclideanRhythm->getCurrentSampleInBar() + numSamples) %
        (euclideanRhythm->get_steps() * euclideanRhythm->getStepDuration()));//((int)steps->load()*stepDuration);

    //TODO cerrar notas en funcino de noteDuration
    
    euclideanRhythm->setTimeStep((euclideanRhythm->getTimeStep() + numSamples) %
        euclideanRhythm->getStepDuration());

}












