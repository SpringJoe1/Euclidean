/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#define NUM_TOTAL_ETAPAS 4

#include <JuceHeader.h>
#include <vector>
#include <cmath> 
#include <string>
#include "EuclideanRhythmComponent.h"
#include <map>

using namespace std;


//==============================================================================
/**
*/
class EucSeq_MultiStageAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    EucSeq_MultiStageAudioProcessor();
    ~EucSeq_MultiStageAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================

    // Audio Processor Value Tree State
    juce::AudioProcessorValueTreeState apvts;

    //==============================================================================

    void setNewNoteNumber(int note, int seqID);
    void setNewNoteDuration(float duration, int seqID);
    void setNewStepDuration(float duration, int seqID);


    //==============================================================================

    //guarradas
    bool guarrada1;

private:

    //==============================================================================

    void initNotesOnMap();
    void convertBPMToTime(EuclideanRhythmComponent* e);
    int getBPM();
    int getCurrentSampleUpdated(int numSamplesPerBar, int newNumSamplesPerBar, int currentSamplesInBar);
    int getIndexFromCurrentSample(EuclideanRhythmComponent* e);

    void processSequencer(juce::MidiBuffer& midiMessages, EuclideanRhythmComponent* euclideanRhythm, int sequencerID);

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();


    //==============================================================================
    
    // notas - numero de noteOns
    map<int, int> notesOn;

    int numTotalSequencers;
    map<int, EuclideanRhythmComponent*> euclideanRhythms;
    
    int numSamples;
    int bpm;
    float rate;
    int midiChannel = 10;
    int velocity;

    juce::AudioPlayHead* playHead;
    juce::AudioPlayHead::CurrentPositionInfo currentPositionInfo;

    // rhythm1
    //EuclideanRhythmComponent* euclideanRhythmComponent;
    //int index;
    //int rotationValue;
    //int noteNumber;
    //// map con el currentNoteNumber y num de samples que lleva sonando (parar en >= noteDuration)
    //map<int, int> notesDurationMap;
    //vector<int> notesToDeleteFromMap;


    //// sample rate


    //// duracion de las notas (blanca, corchea, etc)
    //float figureStep, figureNote;
    //// duraci�n de las notas en n�mero de samples
    //int stepDuration, noteDuration;
    //// contadores para llevar cuanto lleva sonando el step y la nota actual (en n�mero de samples)
    //int timeStep, timeNote;
    //// total de samples de un comp�s para calcular por donde va la aguja
    //int numSamplesPerBar;
    //// n�mero del sample que acabamos de procesar en el compas
    //// int currentSampleInBar;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EucSeq_MultiStageAudioProcessor)
};