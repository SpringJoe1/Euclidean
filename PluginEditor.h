/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#define DEFAULT_PRESET_COMBOBOX 99

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace std;

//==============================================================================
/**
*/
class EuclideanSequencerAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::ComboBox::Listener,
    public juce::Button::Listener
    //public juce::Timer
{
public:
   EuclideanSequencerAudioProcessorEditor(EuclideanSequencerAudioProcessor&);
    ~EuclideanSequencerAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    //void timerCallback() override;

private:

    //==============================================================================

    void paintTrigger();
    void paintRhythm(juce::Graphics& g,int seqID, float innerCircleProp);
    map<int, int> indexes;

    //==============================================================================
    
    void setTextButtonParams(juce::TextButton& textButton, string id);
    void setSliderParams(juce::Slider& slider);
    void setNoteNumberComboBoxParams(juce::ComboBox& comboBox, string id);
    void setDurationComboBoxParams(juce::ComboBox& comboBox, string id);
    void setPresetComboBoxParams(juce::ComboBox& comboBox, string id);

    void disableComponents(int id);
    void enableComponents(int id);

    //==============================================================================

    // Listeners 
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void buttonClicked(juce::Button* button) override;

    //==============================================================================

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    EuclideanSequencerAudioProcessor& audioProcessor;

    juce::TextButton* syncButton;
    juce::TextButton* savePresetButton;
    juce::TextButton* loadPresetButton;
    juce::ComboBox* presetComboBox;

    map<int, juce::TextButton*> onOffButtons;
    map<int, juce::TextButton*> reverseButtons;
    map<int, juce::TextButton*> pingPongButtons;
    map<int, juce::TextButton*> tripletsButtons;
    map<int, juce::TextButton*> dottedButtons;

    map<int, juce::Slider*> stepsSliders;
    map<int, juce::Slider*> eventsSliders;
    map<int, juce::Slider*> rotationSliders;
    map<int, juce::Slider*> velocitySliders;
    map<int, juce::Slider*> gateSliders;

    map<int, juce::ComboBox*> noteNumberComboBoxes;
    map<int, juce::ComboBox*> stepDurationComboBoxes;

    // alias para que sea mas legible
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    map<int, unique_ptr<SliderAttachment>> stepsSliderAttachments;
    map<int, unique_ptr<SliderAttachment>> eventsSliderAttachments;
    map<int, unique_ptr<SliderAttachment>> rotationSliderAttachments;
    map<int, unique_ptr<SliderAttachment>> velocitySliderAttachments;
    map<int, unique_ptr<SliderAttachment>> gateSliderAttachments;

    map<int, unique_ptr<ButtonAttachment>> onOffButtonAttachments;
    map<int, unique_ptr<ButtonAttachment>> reverseButtonAttachments;
    map<int, unique_ptr<ButtonAttachment>> pingPongButtonAttachments;
    map<int, unique_ptr<ButtonAttachment>> tripletsButtonAttachments;
    map<int, unique_ptr<ButtonAttachment>> dottedButtonAttachments;

    map<int, unique_ptr<ButtonAttachment>> shietButtonAttachments;

    map<int, unique_ptr<ComboBoxAttachment>> noteNumberComboBoxAttachments;
    map<int, unique_ptr<ComboBoxAttachment>> stepDurationComboBoxAttachments;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EuclideanSequencerAudioProcessorEditor)
};