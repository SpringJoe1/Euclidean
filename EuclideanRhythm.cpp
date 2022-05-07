/*
  ==============================================================================

    EuclideanRhythm.cpp
    Created: 31 Mar 2022 6:32:07pm
    Author:  dburgos

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EuclideanRhythm.h"

//==============================================================================
// funciones by default del component
//==============================================================================

EuclideanRhythm::EuclideanRhythm()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->_euclideanRhythm = { NULL };
    this->_steps = 1;
    this->_events = 0;
    this->_rotation = 0;
    _euclideanRhythm[0] = 0;
}

EuclideanRhythm::EuclideanRhythm(float rate, int bpmParam, int steps, int events,
    int rotationParam, int velocityParam, int gateParam, int noteNumberParam, int figureStepParam, 
    bool directionParam, bool reverseParam, bool pingPongParam, bool dottedNotesParam, bool tripletsParam,
    int channel) {

    this->sampleRate = rate;
    this->bpm = bpmParam;

    this->_rotation = rotationParam;
    this->_velocity = velocityParam;
    this->_figureStep = figureStepParam;
    this->_gate = gateParam;
    this->_direction = directionParam;
    this->_reverse = reverseParam;
    this->_pingPong = pingPongParam;
    this->_dottedNotes = dottedNotesParam;
    this->_triplets = tripletsParam;
    this->_channel = channel;
    convertBPMToTime();
    this->timeStep = 0;
    this->timeNote = 0;
    this->index = 0;
    this->_noteNumber = noteNumberParam;   // C4
    this->numSamplesPerBar = 0;
    this->currentSampleInBar = 0;

    set_euclideanRhythm(steps, events);
}

EuclideanRhythm::~EuclideanRhythm()
{
}

void EuclideanRhythm::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    g.fillAll(juce::Colours::pink);
}

void EuclideanRhythm::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}


//==============================================================================
// getters and setters
//==============================================================================

// Atributos principales

int EuclideanRhythm::get_steps() {
    return this->_steps;
}

int EuclideanRhythm::get_events() {
    return this->_events;
}

vector<int> EuclideanRhythm::get_euclideanRhythm() {
    return _euclideanRhythm;
}

int EuclideanRhythm::get_rotation() {
    return this->_rotation;
}

int EuclideanRhythm::get_velocity() {
    return this->_velocity;
}

int EuclideanRhythm::get_gate() {
    return this->_gate;
}

bool EuclideanRhythm::get_direction() {
    return this->_direction;
}

bool EuclideanRhythm::get_reverse() {
    return this->_reverse;
}

bool EuclideanRhythm::get_pingPong() {
    return this->_pingPong;
}

bool EuclideanRhythm::get_dottedNotes() {
    return this->_dottedNotes;
}

bool EuclideanRhythm::get_triplets() {
    return this->_triplets;
}

int EuclideanRhythm::get_channel() {
    return this->_channel;
}

juce::String EuclideanRhythm::getList() {
    juce::String ret;
    vector<int>::iterator it = this->_euclideanRhythm.begin();
    for (it = this->_euclideanRhythm.begin(); it != this->_euclideanRhythm.end(); ++it)
        ret += (*it);
    return ret;
}

void EuclideanRhythm::set_euclideanRhythm(int steps, int events) {
    calculateEuclideanRhythm(steps, events);
    rotateRight(this->_rotation);
}

void EuclideanRhythm::calculateEuclideanRhythm(int steps, int events) {

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

void EuclideanRhythm::set_events(int newEvents) {
    set_euclideanRhythm(this->_steps, newEvents);
}

void EuclideanRhythm::set_steps(int newSteps) {
    set_euclideanRhythm(newSteps, this->_events);
}

void EuclideanRhythm::set_rotation(int value) {
    this->_rotation = value;
}

void EuclideanRhythm::set_velocity(int value) {
    this->_velocity = value;
}

void EuclideanRhythm::set_gate(int value) {
    this->_gate = value;
}

void EuclideanRhythm::set_direction(bool value) {
    this->_direction = value;
}

void EuclideanRhythm::set_reverse(bool value) {
    this->_reverse = value;
}

void EuclideanRhythm::set_pingPong(bool value) {
    this->_pingPong = value;
}

void EuclideanRhythm::set_dottedNotes(bool value) {
    this->_dottedNotes = value;
}

void EuclideanRhythm::set_triplets(bool value) {
    this->_triplets = value;
}

void EuclideanRhythm::set_channel(int value) {
    this->_channel = value;
}

// Atributos auxiliares para realizar el processBlock() 

int EuclideanRhythm::getIndex() {
    return this->index;
}

int EuclideanRhythm::get_noteNumber() {
    return this->_noteNumber;
}

float EuclideanRhythm::get_figureStep() {
    return this->_figureStep;
}

int EuclideanRhythm::getStepDuration() {
    return this->stepDuration;
}

int EuclideanRhythm::getNoteDuration() {
    return this->noteDuration;
}

int EuclideanRhythm::getTimeStep() {
    return this->timeStep;
}

int EuclideanRhythm::getTimeNote() {
    return this->timeNote;
}

int EuclideanRhythm::getNumSamplesPerBar() {
    return this->numSamplesPerBar;
}

int EuclideanRhythm::getCurrentSampleInBar() {
    return this->currentSampleInBar;
}

int EuclideanRhythm::getBpm() {
    return this->bpm;
}

float EuclideanRhythm::getRate() {
    return this->sampleRate;
}

void EuclideanRhythm::setIndex(int value) {
    this->index = value;
}

void EuclideanRhythm::set_noteNumber(int value) {
    this->_noteNumber = value;
}

void EuclideanRhythm::set_figureStep(float value) {
    this->_figureStep = value;
}

void EuclideanRhythm::setStepDuration(int value){
    this->stepDuration = value;
}

void EuclideanRhythm::setNoteDuration(int value) {
    this->noteDuration = value;
}

void EuclideanRhythm::setTimeStep(int value) {
    this->timeStep = value;
}

void EuclideanRhythm::setTimeNote(int value) {
    this->timeNote = value;
}

void EuclideanRhythm::setNumSamplesPerBar(int samples) {
    this->numSamplesPerBar = samples;
}

void EuclideanRhythm::setCurrentSampleInBar(int value) {
    this->currentSampleInBar = value;
}


void EuclideanRhythm::setBpm(int value) {
    this->bpm = value;
}

void EuclideanRhythm::setRate(float value) {
    this->sampleRate = value;
}

//==============================================================================
// other aux functions
//==============================================================================

void EuclideanRhythm::rotateRight(int times) {
    for (int i = 0; i < times; i++) {
        int last = this->_euclideanRhythm.back();
        vector<int>::iterator it = this->_euclideanRhythm.begin();
        this->_euclideanRhythm.insert(it, last);
        this->_euclideanRhythm.pop_back();
    }
}

void EuclideanRhythm::rotateLeft(int times) {
    for (int i = 0; i < times; i++) {
        int first = this->_euclideanRhythm.front();
        this->_euclideanRhythm.push_back(first);
        this->_euclideanRhythm.erase(this->_euclideanRhythm.begin());
    }
}

void EuclideanRhythm::showList() {
    vector<int>::iterator it = this->_euclideanRhythm.begin();
    for (it = this->_euclideanRhythm.begin(); it != this->_euclideanRhythm.end(); ++it)
        cout << *it << " ";
    cout << endl << endl;
}

void EuclideanRhythm::switchDirection() {
    if (this->_direction)
        this->_direction = false;
    else
        this->_direction = true;
}




// esta funcion saca el tiempo de nota y de step (en samples) en funcion de los bpms
void EuclideanRhythm::convertBPMToTime() {

    this->setNoteDuration(round((((float)(60000 * this->sampleRate * (this->get_figureStep()* ((float)this->get_gate()/100.0f))) / this->bpm) / 1000) * 100) / 100);
    this->setStepDuration(round((((float)(60000 * this->sampleRate * this->get_figureStep()) / this->bpm) / 1000) * 100) / 100);

}
