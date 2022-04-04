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
#include "EuclideanRythmComponent.h"
#include <map>

using namespace std;


//==============================================================================
/**
*/
class Seq_v4AudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    Seq_v4AudioProcessor();
    ~Seq_v4AudioProcessor() override;

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

    void convertBPMToTime(EuclideanRythmComponent* e);
    int getBPM();
    int getCurrentSampleUpdated(int numSamplesPerBar, int newNumSamplesPerBar, int currentSamplesInBar);
    int getIndexFromCurrentSample(EuclideanRythmComponent* e);

    void processSequencer(juce::MidiBuffer& midiMessages, EuclideanRythmComponent* euclideanRythm, int sequencerID);

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();


    //==============================================================================


    int numTotalSequencers;
    map<int, EuclideanRythmComponent*> euclideanRythms;
    int numSamples;
    int bpm;

    juce::AudioPlayHead* playHead;
    juce::AudioPlayHead::CurrentPositionInfo currentPositionInfo;

    // rythm1
    EuclideanRythmComponent* euclideanRythmComponent;
    int index;
    int rotationValue;
    int noteNumber;
    // map con el currentNoteNumber y num de samples que lleva sonando (parar en >= noteDuration)
    map<int, int> notesDurationMap;
    vector<int> notesToDeleteFromMap;


    // sample rate
    float rate;
    int midiChannel = 10;
    int velocity;

    // duracion de las notas (blanca, corchea, etc)
    float figureStep, figureNote;
    // duraci�n de las notas en n�mero de samples
    int stepDuration, noteDuration;
    // contadores para llevar cuanto lleva sonando el step y la nota actual (en n�mero de samples)
    int timeStep, timeNote;
    // total de samples de un comp�s para calcular por donde va la aguja
    int numSamplesPerBar;
    // n�mero del sample que acabamos de procesar en el compas
    // int currentSampleInBar;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Seq_v4AudioProcessor)
};