/*
  ==============================================================================

    EuclideanRythmComponent.cpp
    Created: 31 Mar 2022 6:32:07pm
    Author:  dburgos

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EuclideanRythmComponent.h"

//==============================================================================
// funciones by default del component
//==============================================================================

EuclideanRythmComponent::EuclideanRythmComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->_euclideanRythm = { NULL };
    this->_steps = 0;
    this->_events = 0;
    this->_rotation = 0;
    _euclideanRythm[0] = 0;
}

EuclideanRythmComponent::EuclideanRythmComponent(int steps, int events, int rotation) {
    set_euclideanRythm(steps, events, rotation);
}

EuclideanRythmComponent::~EuclideanRythmComponent()
{
}

void EuclideanRythmComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

}

void EuclideanRythmComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


//==============================================================================
// getters and setters
//==============================================================================

// Atributos principales

int EuclideanRythmComponent::get_steps() {
    return this->_steps;
}

int EuclideanRythmComponent::get_events() {
    return this->_events;
}

vector<int> EuclideanRythmComponent::get_euclideanRythm() {
    return _euclideanRythm;
}

int EuclideanRythmComponent::get_rotation() {
    return this->_rotation;
}

juce::String EuclideanRythmComponent::getList() {
    juce::String ret;
    vector<int>::iterator it = this->_euclideanRythm.begin();
    for (it = this->_euclideanRythm.begin(); it != this->_euclideanRythm.end(); ++it)
        ret += (*it);
    return ret;
}

void EuclideanRythmComponent::set_euclideanRythm(int steps, int events, int rotation) {

    this->_euclideanRythm.clear();

    this->_steps = steps;
    this->_events = events;
    if(this->_rotation < 0 || this->_rotation > 16)
        this->_rotation = rotation;

    if (events == 0) {
        for (int i = 0; i < this->_steps; i++)
            this->_euclideanRythm.push_back(0);
        rotateRight(rotation);
        return;
    }

    int previous = 0;
    this->_euclideanRythm.push_back(1);

    for (int i = 1; i < steps; i++) {
        int currentValue = floor(((double)events / steps) * i);
        if (currentValue == previous)
            this->_euclideanRythm.push_back(0);
        else
            this->_euclideanRythm.push_back(1);
        previous = currentValue;
    }

    rotateRight(rotation);
}

void EuclideanRythmComponent::set_events(int newEvents) {
    set_euclideanRythm(this->_steps, newEvents, _rotation);
}

void EuclideanRythmComponent::set_steps(int newSteps) {
    set_euclideanRythm(newSteps, this->_events, _rotation);
}

void EuclideanRythmComponent::set_rotation(int value) {
    this->_rotation = value;
}

// Atributos auxiliares para realizar el processBlock() 

int EuclideanRythmComponent::getIndex() {
    return this->index;
}

int EuclideanRythmComponent::getRotationValue() {
    return this->rotationValue;
}

int EuclideanRythmComponent::getNoteNumber() {
    return this->noteNumber;
}

map<int, int> EuclideanRythmComponent::getNotesDurationMap() {
    return this->notesDurationMap;
}

vector<int> EuclideanRythmComponent::getNotesToDeleteFromMap() {
    return this->notesToDeleteFromMap;
}

float EuclideanRythmComponent::getFigureStep() {
    return this->figureStep;
}

float EuclideanRythmComponent::getFigureNote() {
    return this->figureNote;
}

int EuclideanRythmComponent::getStepDuration() {
    return this->stepDuration;
}

int EuclideanRythmComponent::getNoteDuration() {
    return this->noteDuration;
}

int EuclideanRythmComponent::getTimeStep() {
    return this->timeStep;
}

int EuclideanRythmComponent::getTimeNote() {
    return this->timeNote;
}

int EuclideanRythmComponent::getNumSamplesPerBar() {
    return this->numSamplesPerBar;
}

int EuclideanRythmComponent::getCurrentSampleInBar() {
    return this->currentSampleInBar;
}

void EuclideanRythmComponent::setIndex(int value) {
    this->index = value;
}

void EuclideanRythmComponent::setRotationValue(int value) {
    this->rotationValue = value;
}

void EuclideanRythmComponent::setNoteNumber(int value) {
    this->noteNumber = value;
}

//void setNotesDurationMap();
//void setNotesToDeleteFromMap();

void EuclideanRythmComponent::setFigureStep(float value) {
    this->figureStep = value;
}
void EuclideanRythmComponent::setFigureNote(float value) {
    this->figureNote = value;
}

void EuclideanRythmComponent::setStepDuration(int value){
    this->stepDuration = value;
}
void EuclideanRythmComponent::setNoteDuration(int value) {
    this->noteDuration = value;
}

void EuclideanRythmComponent::setTimeStep(int value) {
    this->timeStep = value;
}

void EuclideanRythmComponent::setTimeNote(int value) {
    this->timeNote = value;
}

void EuclideanRythmComponent::setNumSamplesPerBar(int samples) {
    this->numSamplesPerBar = samples;
}

void EuclideanRythmComponent::setCurrentSampleInBar(int value) {
    this->currentSampleInBar = value;
}

//==============================================================================
// other aux functions
//==============================================================================

void EuclideanRythmComponent::rotateRight(int times) {
    for (int i = 0; i < times; i++) {
        int last = this->_euclideanRythm.back();
        vector<int>::iterator it = this->_euclideanRythm.begin();
        this->_euclideanRythm.insert(it, last);
        this->_euclideanRythm.pop_back();
    }
    this->_rotation += times;
}

void EuclideanRythmComponent::rotateLeft(int times) {
    for (int i = 0; i < times; i++) {
        int first = this->_euclideanRythm.front();
        this->_euclideanRythm.push_back(first);
        this->_euclideanRythm.erase(this->_euclideanRythm.begin());
    }
    this->_rotation -= times;
}

void EuclideanRythmComponent::showList() {
    vector<int>::iterator it = this->_euclideanRythm.begin();
    for (it = this->_euclideanRythm.begin(); it != this->_euclideanRythm.end(); ++it)
        cout << *it << " ";
    cout << endl << endl;
}

