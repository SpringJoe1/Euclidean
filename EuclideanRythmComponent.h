/*
  ==============================================================================

    EuclideanRythmComponent.h
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
class EuclideanRythmComponent  : public juce::Component
{
public:

    //==============================================================================
    // funciones by default del component
    //==============================================================================

    EuclideanRythmComponent();
    EuclideanRythmComponent(int steps, int events, int rotation);
    ~EuclideanRythmComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    // getters and setters
    //==============================================================================

    // Atributos principales 
    int getSteps();
    int getEvents();
    vector<int> getEuclideanRythm();
    juce::String getList();
    void setEuclideanRythm(int steps, int events, int rotation);
    void setEvents(int newEvents);
    void setSteps(int newSteps);

    // Atributos auxiliares para realizar el processBlock() 


    //==============================================================================
    // other aux functions
    //==============================================================================

    void rotateRight(int times);
    void rotateLeft(int times);
    void showList();


private:

    //==============================================================================
    // Atributos principales
    //==============================================================================

    vector<int> _euclideanRythm;
    int _steps;
    int _events;
    int _rotation;

    //==============================================================================
    // Atributos auxiliares para realizar el processBlock() 
    //==============================================================================

    // indice de la nota que va a ser procesada
    int index;
    // valor de la rotacion actual
    int rotationValue;
    // valor de la nota midi (entre C0 y B6 que son la 24 y 106)
    int noteNumber;
    
    // map con el currentNoteNumber y num de samples que lleva sonando (parar en >= noteDuration)
    map<int, int> notesDurationMap;
    vector<int> notesToDeleteFromMap;

    // duracion de las notas (blanca, corchea, etc)
    float figureStep, figureNote;
    // duracion de las notas en numero de samples
    int stepDuration, noteDuration;
    // contadores para llevar cuanto lleva sonando el step y la nota actual (en numero de samples)
    int timeStep, timeNote;
    // total de samples de un compas para calcular por donde va la aguja
    int numSamplesPerBar;
    // n�mero del sample que acabamos de procesar en el compas
    int currentSampleInBar;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EuclideanRythmComponent)
};
