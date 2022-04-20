/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#define NUM_TOTAL_ETAPAS 4
#define CONST_DURATION_TIME_CONV 1000


#include <JuceHeader.h>
#include <vector>
#include <cmath> 
#include <string>
#include "EuclideanRhythm.h"
#include <map>
#include <mutex>

using namespace std;


//==============================================================================
/**
*/
class EuclideanSequencerAudioProcessor : public juce::AudioProcessor
{
public:

    // mutex que controla la sincronia
    mutex my_mutex[NUM_TOTAL_ETAPAS];



    //==============================================================================
    EuclideanSequencerAudioProcessor();
    ~EuclideanSequencerAudioProcessor() override;

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

    void setNewNoteNumber(int value, int seqID);
    void setNewStepFigure(float duration, int seqID);
    void createRythm(int id, int steps, int events, int rotation, int velocity, int gate,
        int noteNumber, float figureStep, bool direction, bool reverse, bool pingPong,
        bool dottedNotesParam, bool tripletsParam);
    void deleteRythm(int id);
    void setReverseDirection(int id, bool reverse);
    void setNewPingPong(int id, bool random);
    void synchronizeAll();
    void setDottedNotes(int seqID, bool value, float newDuration);
    void setTriplets(int seqID, bool value, float newDuration);
    void savePreset();
    void loadPreset();

    map<int, EuclideanRhythm*> getEuclideanRhythms();

    //==============================================================================

private:

    //==============================================================================

    void initNotesOnMap();
    //void convertBPMToTime(EuclideanRhythmComponent* e);
    int getBPM();
    int getCurrentSampleUpdated(int numSamplesPerBar, int newNumSamplesPerBar, int currentSamplesInBar);
    int getIndexFromCurrentSample(EuclideanRhythm* e);

    void processSequencer(juce::MidiBuffer& midiMessages, EuclideanRhythm* euclideanRhythm, int sequencerID);

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();


    //==============================================================================
    
    // mapa de <ID, puntero a EuclideanRhythmComponent> donde estarán los 4 
    map<int, EuclideanRhythm*> euclideanRhythms;
    
    // mapa de <notas - numero de noteOns> global
    map<int, int> notesOn;
    
    int numSamples;
    int bpm;
    float rate;
    int midiChannel = 10;


    // to get information from the application 
    juce::AudioPlayHead* playHead;
    juce::AudioPlayHead::CurrentPositionInfo currentPositionInfo;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EuclideanSequencerAudioProcessor)
};