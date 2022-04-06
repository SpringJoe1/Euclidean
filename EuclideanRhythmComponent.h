/*
  ==============================================================================

    EuclideanRhythmComponent.h
    Created: 31 Mar 2022 6:32:07pm
    Author:  dburgos

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <cmath>
#include <map>

using namespace std;


//==============================================================================
/*
*/
class EuclideanRhythmComponent  : public juce::Component
{
public:

    //==============================================================================
    // funciones by default del component
    //==============================================================================

    EuclideanRhythmComponent();
    EuclideanRhythmComponent(int steps, int events, float rate, int bpmParam, int rotationParam = 0,
        int velocityParam = 127, int figureStepParam = 1, int figureNoteParam = 1, int noteNumberParam = 72);
    ~EuclideanRhythmComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    // getters and setters
    //==============================================================================

    // Atributos principales 
    int get_steps();
    int get_events();
    int get_rotation();
    int get_velocity();
    vector<int> get_euclideanRhythm();
    juce::String getList();

    void calculateEuclideanRhythm(int steps, int events);
    void set_euclideanRhythm(int steps, int events);
    void set_events(int newEvents);
    void set_steps(int newSteps);
    void set_rotation(int rotation);
    void set_velocity(int value);

    // Atributos auxiliares para realizar el processBlock() 
    int getIndex();
    int getNoteNumber();
    map<int, int> getNotesDurationMap();
    vector<int> getNotesToDeleteFromMap();
    float getFigureStep();
    float getFigureNote();
    int getStepDuration();
    int getNoteDuration();
    int getTimeStep();
    int getTimeNote();
    int getNumSamplesPerBar();
    int getCurrentSampleInBar();

    void setIndex(int value);
    void setNoteNumber(int value);
    //void setNotesDurationMap();
    //void setNotesToDeleteFromMap();
    void setFigureStep(float value);
    void setFigureNote(float value);
    void setStepDuration(int value);
    void setNoteDuration(int value);
    void setTimeStep(int value);
    void setTimeNote(int value);
    void setNumSamplesPerBar(int samples);
    void setCurrentSampleInBar(int value);

    //==============================================================================
    // other aux functions
    //==============================================================================

    void rotateRight(int times);
    void rotateLeft(int times);
    void showList();
    // esta funcion saca el tiempo de nota y de step (en samples) en funcion de los bpms
    void convertBPMToTime();

    //==============================================================================
    // TODO -- Atributos publicos (cambiar a private)
    //==============================================================================

    int bpm;
    float sampleRate;

    // map con el currentNoteNumber y num de samples que lleva sonando (parar en >= noteDuration)
    map<int, int> notesDurationMap;
    vector<int> notesToDeleteFromMap;

private:

    //==============================================================================
    // Atributos principales
    //==============================================================================

    vector<int> _euclideanRhythm;
    int _steps;
    int _events;
    int _rotation;
    int _velocity;

    //==============================================================================
    // Atributos auxiliares para realizar el processBlock() 
    //==============================================================================

    // indice de la nota que va a ser procesada
    int index;
    // valor de la rotacion actual
    int rotationValue;
    // valor de la nota midi (entre C0 y B6 que son la 24 y 106)
    int noteNumber;
    


    // duracion de las notas (blanca, corchea, etc)
    float figureStep, figureNote;
    // duracion de las notas en numero de samples
    int stepDuration, noteDuration;
    // contadores para llevar cuanto lleva sonando el step y la nota actual (en numero de samples)
    int timeStep, timeNote;
    // total de samples de un compas para calcular por donde va la aguja
    int numSamplesPerBar;
    // numero del sample que acabamos de procesar en el compas
    int currentSampleInBar;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EuclideanRhythmComponent)
};
