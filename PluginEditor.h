/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#define CONST_DURATION_TIME_CONV 1000

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace std;

//==============================================================================
/**
*/
class EucSeq_MultiStageAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::ComboBox::Listener,
    public juce::Button::Listener
{
public:
    EucSeq_MultiStageAudioProcessorEditor(EucSeq_MultiStageAudioProcessor&);
    ~EucSeq_MultiStageAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:

    //==============================================================================

    //void paintRhythm(juce::Graphics& g);

    //==============================================================================
    
    void setTextButtonParams(juce::TextButton& textButton, string id);
    void setSliderParams(juce::Slider& slider);
    void setNoteNumberComboBoxParams(juce::ComboBox& comboBox, string id);
    void setDurationComboBoxParams(juce::ComboBox& comboBox, string id);

    void disableComponents(int id);
    void enableComponents(int id);

    //==============================================================================

    // Listeners 
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button* button) override;

    //==============================================================================

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EucSeq_MultiStageAudioProcessor& audioProcessor;

    // alias para que sea mas legible
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    map<int, juce::TextButton*> onOffButtons;
    map<int, juce::TextButton*> reverseButtons;
    map<int, juce::TextButton*> pingPongButtons;

    juce::TextButton* syncButton;

    map<int, juce::Slider*> stepsSliders;
    map<int, juce::Slider*> eventsSliders;
    map<int, juce::Slider*> rotationSliders;
    map<int, juce::Slider*> velocitySliders;
    map<int, juce::Slider*> gateSliders;

    map<int, unique_ptr<SliderAttachment>> stepsSliderAttachments;
    map<int, unique_ptr<SliderAttachment>> eventsSliderAttachments;
    map<int, unique_ptr<SliderAttachment>> rotationSliderAttachments;
    map<int, unique_ptr<SliderAttachment>> velocitySliderAttachments;
    map<int, unique_ptr<SliderAttachment>> gateSliderAttachments;

    map<int, juce::ComboBox*> noteNumberComboBoxes;
    map<int, juce::ComboBox*> stepDurationComboBoxes;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EucSeq_MultiStageAudioProcessorEditor)
};