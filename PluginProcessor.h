/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <cmath> 
#include "EuclideanRythm.h"

using namespace std;

// From midinote 21 to 107
//const juce::StringArray noteNames = ("C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0",
//    "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1", 
//    "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2", 
//    "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3", 
//    "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4", 
//    "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5", 
//    "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6");


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

    void setNewNoteNumber(int note);
    EuclideanRythm getEuclideanRythm();

    //==============================================================================

    //guarradas
    bool guarrada1;

private:

    //==============================================================================

    void convertBPMToTime();
    // funcion que saca el angulo de la aguja a partir del sample en el que estamos

    // funciones alv
    //float getAngleFromCurrentSample();
    //// funcion que nos devuelve el indice de la nota que tiene que sonar en funcion del
    //// angulo de la aguja
    //int getIndexFromAngle();
    //// funcion que nos devuelve el currentSampleInBar a partir del angulo de la aguja
    //int getCurrentSamplesFromAngle();
    //// function that returns the parameter layout of the audio processor value tree state

    int getCurrentSampleUpdated(int numSamplesPerBar, int newNumSamplesPerBar);
    int getIndexFromCurrentSample();
    void updateNoteNumber();

    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();


    //==============================================================================

    juce::AudioPlayHead* playHead;
    juce::AudioPlayHead::CurrentPositionInfo currentPositionInfo;

    // rythm1
    EuclideanRythm euclideanRythm;
    int index;
    int rotationValue;
    int currentNoteNumber, newNoteNumber;



    // sample rate
    float rate;
    int midiChannel = 10;
    int velocity;

    // duracion de las notas (blanca, corchea, etc)
    int figureStep, figureNote;
    // duración de las notas en número de samples
    int stepDuration, noteDuration;
    // contadores para llevar cuanto lleva sonando el step y la nota actual (en número de samples)
    int timeStep, timeNote;
    // total de samples de un compás para calcular por donde va la aguja
    int numSamplesPerBar;
    // número del sample que acabamos de procesar en el compas
    int currentSampleInBar;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Seq_v4AudioProcessor)
};
