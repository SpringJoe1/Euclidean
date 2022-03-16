/*
  ==============================================================================

    EuclideanRythm.h
    Created: 15 Mar 2022 4:59:06pm
    Author:  dburgos

  ==============================================================================
*/

#pragma once

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

class EuclideanRythm {
private:
    vector<int> _euclideanRythm;
    int _steps;
    int _events;

    
public:

    EuclideanRythm() {
        this->_euclideanRythm = { NULL };
        this->_steps = 0;
        this->_events = 0;
        _euclideanRythm[0] = 0;
    }

    EuclideanRythm(int steps, int events) {
        setEuclideanRythm(steps, events);
    }

    void setEuclideanRythm(int steps, int events) {

        this->_euclideanRythm.clear();

        this->_steps = steps;
        this->_events = events;

        if (events == 0) {
            for (int i = 0; i < this->_steps; i++)
                this->_euclideanRythm.push_back(0);
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

    }

    vector<int> getEuclideanRythm() {
        return _euclideanRythm;
    }

    void rotate() {
        int last = this->_euclideanRythm.back();
        vector<int>::iterator it = this->_euclideanRythm.begin();
        this->_euclideanRythm.insert(it, last);
        this->_euclideanRythm.pop_back();
    }

    void changeEvents(int newEvents) {
        setEuclideanRythm(this->_steps, newEvents);
    }

    void changeSteps(int newSteps) {
        setEuclideanRythm(newSteps, this->_events);
    }

    void showList() {
        vector<int>::iterator it = this->_euclideanRythm.begin();
        for (it = this->_euclideanRythm.begin(); it != this->_euclideanRythm.end(); ++it)
            cout << *it << " ";
        cout << endl << endl;
    }
};
