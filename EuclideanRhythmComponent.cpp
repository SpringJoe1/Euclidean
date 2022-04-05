/*
  ==============================================================================

    EuclideanRhythmComponent.cpp
    Created: 31 Mar 2022 6:32:07pm
    Author:  dburgos

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EuclideanRhythmComponent.h"

//==============================================================================
// funciones by default del component
//==============================================================================

EuclideanRhythmComponent::EuclideanRhythmComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->_euclideanRhythm = { NULL };
    this->_steps = 0;
    this->_events = 0;
    this->_rotation = 0;
    _euclideanRhythm[0] = 0;
}

EuclideanRhythmComponent::EuclideanRhythmComponent(int steps, int events) {
    set_euclideanRhythm(steps, events);
}

EuclideanRhythmComponent::~EuclideanRhythmComponent()
{
}

void EuclideanRhythmComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

}

void EuclideanRhythmComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


//==============================================================================
// getters and setters
//==============================================================================

// Atributos principales

int EuclideanRhythmComponent::get_steps() {
    return this->_steps;
}

int EuclideanRhythmComponent::get_events() {
    return this->_events;
}

vector<int> EuclideanRhythmComponent::get_euclideanRhythm() {
    return _euclideanRhythm;
}

int EuclideanRhythmComponent::get_rotation() {
    return this->_rotation;
}

juce::String EuclideanRhythmComponent::getList() {
    juce::String ret;
    vector<int>::iterator it = this->_euclideanRhythm.begin();
    for (it = this->_euclideanRhythm.begin(); it != this->_euclideanRhythm.end(); ++it)
        ret += (*it);
    return ret;
}

void EuclideanRhythmComponent::set_euclideanRhythm(int steps, int events) {
    calculateEuclideanRhythm(steps, events);
    rotateRight(this->_rotation);
}

void EuclideanRhythmComponent::calculateEuclideanRhythm(int steps, int events) {

    this->_euclideanRhythm.clear();

    this->_steps = steps;
    this->_events = events;


    if (this->_events == 0) {
        for (int i = 0; i < this->_steps; i++)
            this->_euclideanRhythm.push_back(0);
        return;
    }

    int previous = 0;
    this->_euclideanRhythm.push_back(1);

    for (int i = 1; i < steps; i++) {
        int currentValue = floor(((double)this->_events / this->_steps) * i);
        if (currentValue == previous)
            this->_euclideanRhythm.push_back(0);
        else
            this->_euclideanRhythm.push_back(1);
        previous = currentValue;
    }


}

void EuclideanRhythmComponent::set_events(int newEvents) {
    set_euclideanRhythm(this->_steps, newEvents);
}

void EuclideanRhythmComponent::set_steps(int newSteps) {
    set_euclideanRhythm(newSteps, this->_events);
}

void EuclideanRhythmComponent::set_rotation(int value) {
    this->_rotation = value;
}

// Atributos auxiliares para realizar el processBlock() 

int EuclideanRhythmComponent::getIndex() {
    return this->index;
}

int EuclideanRhythmComponent::getNoteNumber() {
    return this->noteNumber;
}

map<int, int> EuclideanRhythmComponent::getNotesDurationMap() {
    return this->notesDurationMap;
}

vector<int> EuclideanRhythmComponent::getNotesToDeleteFromMap() {
    return this->notesToDeleteFromMap;
}

float EuclideanRhythmComponent::getFigureStep() {
    return this->figureStep;
}

float EuclideanRhythmComponent::getFigureNote() {
    return this->figureNote;
}

int EuclideanRhythmComponent::getStepDuration() {
    return this->stepDuration;
}

int EuclideanRhythmComponent::getNoteDuration() {
    return this->noteDuration;
}

int EuclideanRhythmComponent::getTimeStep() {
    return this->timeStep;
}

int EuclideanRhythmComponent::getTimeNote() {
    return this->timeNote;
}

int EuclideanRhythmComponent::getNumSamplesPerBar() {
    return this->numSamplesPerBar;
}

int EuclideanRhythmComponent::getCurrentSampleInBar() {
    return this->currentSampleInBar;
}

void EuclideanRhythmComponent::setIndex(int value) {
    this->index = value;
}

void EuclideanRhythmComponent::setNoteNumber(int value) {
    this->noteNumber = value;
}

//void setNotesDurationMap();
//void setNotesToDeleteFromMap();

void EuclideanRhythmComponent::setFigureStep(float value) {
    this->figureStep = value;
}
void EuclideanRhythmComponent::setFigureNote(float value) {
    this->figureNote = value;
}

void EuclideanRhythmComponent::setStepDuration(int value){
    this->stepDuration = value;
}
void EuclideanRhythmComponent::setNoteDuration(int value) {
    this->noteDuration = value;
}

void EuclideanRhythmComponent::setTimeStep(int value) {
    this->timeStep = value;
}

void EuclideanRhythmComponent::setTimeNote(int value) {
    this->timeNote = value;
}

void EuclideanRhythmComponent::setNumSamplesPerBar(int samples) {
    this->numSamplesPerBar = samples;
}

void EuclideanRhythmComponent::setCurrentSampleInBar(int value) {
    this->currentSampleInBar = value;
}

//==============================================================================
// other aux functions
//==============================================================================

void EuclideanRhythmComponent::rotateRight(int times) {
    for (int i = 0; i < times; i++) {
        int last = this->_euclideanRhythm.back();
        vector<int>::iterator it = this->_euclideanRhythm.begin();
        this->_euclideanRhythm.insert(it, last);
        this->_euclideanRhythm.pop_back();
    }
}

void EuclideanRhythmComponent::rotateLeft(int times) {
    for (int i = 0; i < times; i++) {
        int first = this->_euclideanRhythm.front();
        this->_euclideanRhythm.push_back(first);
        this->_euclideanRhythm.erase(this->_euclideanRhythm.begin());
    }
}

void EuclideanRhythmComponent::showList() {
    vector<int>::iterator it = this->_euclideanRhythm.begin();
    for (it = this->_euclideanRhythm.begin(); it != this->_euclideanRhythm.end(); ++it)
        cout << *it << " ";
    cout << endl << endl;
}

