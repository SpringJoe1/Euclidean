/*
  ==============================================================================

    EuclideanRythm.h
    Created: 23 Mar 2022 3:36:47pm
    Author:  dburgos

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <vector>
#include <cmath>


using namespace std;

class EuclideanRythm {
private:
    vector<int> _euclideanRythm;
    int _steps;
    int _events;
    int _rotation;

public:

    EuclideanRythm() {
        this->_euclideanRythm = { NULL };
        this->_steps = 0;
        this->_events = 0;
        this->_rotation = 0;
        _euclideanRythm[0] = 0;
    }

    EuclideanRythm(int steps, int events, int rotation) {
        setEuclideanRythm(steps, events, rotation);
    }

    //==============================================================================

    int getSteps() {
        return this->_steps;
    }

    int getEvents() {
        return this->_events;
    }

    vector<int> getEuclideanRythm() {
        return _euclideanRythm;
    }

    juce::String getList() {
        juce::String ret;
        vector<int>::iterator it = this->_euclideanRythm.begin();
        for (it = this->_euclideanRythm.begin(); it != this->_euclideanRythm.end(); ++it)
            ret += (*it);
        return ret;
    }

    //==============================================================================

    void setEuclideanRythm(int steps, int events, int rotation) {

        this->_euclideanRythm.clear();

        this->_steps = steps;
        this->_events = events;

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

    void setEvents(int newEvents) {
        setEuclideanRythm(this->_steps, newEvents, _rotation);
    }

    void setSteps(int newSteps) {
        setEuclideanRythm(newSteps, this->_events, _rotation);
    }

    //==============================================================================

    void rotateRight(int times) {
        for (int i = 0; i < times; i++) {
            int last = this->_euclideanRythm.back();
            vector<int>::iterator it = this->_euclideanRythm.begin();
            this->_euclideanRythm.insert(it, last);
            this->_euclideanRythm.pop_back();
        }
        this->_rotation += times;
    }

    void rotateLeft(int times) {
        for (int i = 0; i < times; i++) {
            int first = this->_euclideanRythm.front();
            this->_euclideanRythm.push_back(first);
            this->_euclideanRythm.erase(this->_euclideanRythm.begin());
        }
        this->_rotation -= times;
    }

    void showList() {
        vector<int>::iterator it = this->_euclideanRythm.begin();
        for (it = this->_euclideanRythm.begin(); it != this->_euclideanRythm.end(); ++it)
            cout << *it << " ";
        cout << endl << endl;
    }
};