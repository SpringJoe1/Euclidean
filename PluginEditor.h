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
    public juce::ComboBox::Listener
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

    void setSliderParams(juce::Slider& slider);
    void setNoteNumberComboBoxParams(juce::ComboBox& comboBox, string id);
    void setDurationComboBoxParams(juce::ComboBox& comboBox, string id);
    //==============================================================================

    // Listener del comboBox 
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    //==============================================================================

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EucSeq_MultiStageAudioProcessor& audioProcessor;

    // alias para que sea mas legible
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    juce::Slider stepsSlider;
    juce::Slider eventsSlider;
    juce::Slider rotationSlider;
    juce::Slider velocitySlider;
    unique_ptr<SliderAttachment> stepsSliderAttachment;
    unique_ptr<SliderAttachment> eventsSliderAttachment;
    unique_ptr<SliderAttachment> rotationSliderAttachment;
    unique_ptr<SliderAttachment> velocitySliderAttachment;

    juce::ComboBox noteNumberComboBox;
    juce::ComboBox noteDurationComboBox;
    juce::ComboBox stepDurationComboBox;

    /////////////////////////////////////////////////////////////////////
    // seq2
    juce::Slider stepsSlider1;
    juce::Slider eventsSlider1;
    juce::Slider rotationSlider1;
    juce::Slider velocitySlider1;
    unique_ptr<SliderAttachment> stepsSliderAttachment1;
    unique_ptr<SliderAttachment> eventsSliderAttachment1;
    unique_ptr<SliderAttachment> rotationSliderAttachment1;
    unique_ptr<SliderAttachment> velocitySliderAttachment1;

    juce::ComboBox noteNumberComboBox1;
    juce::ComboBox noteDurationComboBox1;
    juce::ComboBox stepDurationComboBox1;

    /////////////////////////////////////////////////////////////////////
    // seq3
    juce::Slider stepsSlider2;
    juce::Slider eventsSlider2;
    juce::Slider rotationSlider2;
    juce::Slider velocitySlider2;
    unique_ptr<SliderAttachment> stepsSliderAttachment2;
    unique_ptr<SliderAttachment> eventsSliderAttachment2;
    unique_ptr<SliderAttachment> rotationSliderAttachment2;
    unique_ptr<SliderAttachment> velocitySliderAttachment2;

    juce::ComboBox noteNumberComboBox2;
    juce::ComboBox noteDurationComboBox2;
    juce::ComboBox stepDurationComboBox2;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EucSeq_MultiStageAudioProcessorEditor)
};