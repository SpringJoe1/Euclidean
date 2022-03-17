/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <cmath>
#include <JuceHeader.h>
#include "EuclideanRythm.h"

using namespace std;

//==============================================================================
/**
*/
class Seq_v3AudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Seq_v3AudioProcessor();
    ~Seq_v3AudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

    // Audio Processor Value Tree State
    juce::AudioProcessorValueTreeState apvts;

    //==============================================================================

private:

    //==============================================================================

    void convertBPMToTime();
    // function that returns the parameter layout of the audio processor value tree state
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();


    //==============================================================================

    juce::AudioPlayHead* playHead;
    juce::AudioPlayHead::CurrentPositionInfo currentPositionInfo;


    EuclideanRythm euclideanRythm;
    int index;

    // sample rate
    float rate;
    int noteNumber = 48;
    int midiChannel = 10;
    int velocity;

    // duracion de las notas (blanca, corchea, etc)
    int figureStep, figureNote;
    // duración de las notas en número de samples
    int stepDuration, noteDuration;
    // contadores para llevar cuanto lleva sonando el step y la nota actual (en número de samples)
    int timeStep, timeNote;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Seq_v3AudioProcessor)
};
