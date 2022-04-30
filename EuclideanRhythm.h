/*
  ==============================================================================

    EuclideanRhythm.h
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
class EuclideanRhythm  : public juce::Component
{
public:

    //==============================================================================
    // funciones by default del component
    //==============================================================================

    EuclideanRhythm();
    EuclideanRhythm(float rate, int bpmParam, int steps, int events, int rotationParam = 0,
        int velocityParam = 127, int gateParam = 100, int noteNumberParam = 72,
        int figureStepParam = 1, bool direction = true, bool reverse = false, bool pingPong = false,
        bool dottedNotesParam = false, bool tripletsParam = false);
    ~EuclideanRhythm() override;

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
    int get_gate();
    int get_noteNumber();
    float get_figureStep();
    bool get_direction();
    bool get_reverse();
    bool get_pingPong();
    bool get_dottedNotes();
    bool get_triplets();
    int get_channel();
    vector<int> get_euclideanRhythm();
    juce::String getList();

    void calculateEuclideanRhythm(int steps, int events);
    void set_euclideanRhythm(int steps, int events);
    void set_events(int newEvents);
    void set_steps(int newSteps);
    void set_rotation(int rotation);
    void set_velocity(int value);
    void set_gate(int value);
    void set_noteNumber(int value);
    void set_figureStep(float value);
    void set_direction(bool value);
    void set_reverse(bool value);
    void set_pingPong(bool value);
    void set_dottedNotes(bool value);
    void set_triplets(bool value);
    void set_channel(int newChannel);

    // Atributos auxiliares para realizar el processBlock() 
    int getIndex();
    int getStepDuration();
    int getNoteDuration();
    int getTimeStep();
    int getTimeNote();
    int getNumSamplesPerBar();
    int getCurrentSampleInBar();

    int getBpm();
    float getRate();

    void setIndex(int value);
    void setStepDuration(int value);
    void setNoteDuration(int value);
    void setTimeStep(int value);
    void setTimeNote(int value);
    void setNumSamplesPerBar(int samples);
    void setCurrentSampleInBar(int value);

    void setBpm(int value);
    void setRate(float rate);

    //==============================================================================
    // other aux functions
    //==============================================================================

    void rotateRight(int times);
    void rotateLeft(int times);
    void showList();
    // esta funcion saca el tiempo de nota y de step (en samples) en funcion de los bpms
    void convertBPMToTime();
    void switchDirection();


    //==============================================================================
    // TODO -- Atributos publicos (cambiar a private)
    //==============================================================================
    // map con el currentNoteNumber y num de samples que lleva sonando (parar en >= noteDuration)
    map<int, int> notesDurationMap;
    //map con el current noteNumber y num de canal de midi al que se mandó
    map<int, int> notesChannelMap;
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
    int _gate;
    int _channel;
    // valor de la nota midi (entre C0 y B6 que son la 24 y 106)
    int _noteNumber;
    // duracion de los steps (blanca, corchea, etc)
    float _figureStep;
    // _direction = true if the direction goes to the right.
    bool _direction;
    // _reverse = true to reverse the direction.
    bool _reverse;
    // _pingPong = true => pingPong mode On
    bool _pingPong;
    // _dottedNotes = true => dottesNotes On
    bool _dottedNotes;
    // _triplets = true => triplets On
    bool _triplets;

    //==============================================================================
    // Atributos auxiliares para realizar el processBlock() 
    //==============================================================================

    int bpm;
    float sampleRate;
    
    // indice de la nota que va a ser procesada
    int index;

    // duracion de las notas en numero de samples
    int stepDuration, noteDuration;
    // contadores para llevar cuanto lleva sonando el step y la nota actual (en numero de samples)
    int timeStep, timeNote;
    // total de samples de un compas para calcular por donde va la aguja
    int numSamplesPerBar;
    // numero del sample que acabamos de procesar en el compas
    int currentSampleInBar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EuclideanRhythm)
};