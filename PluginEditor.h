/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace std;

//==============================================================================
/**
*/
class Seq_v4AudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::ComboBox::Listener
{
public:
    Seq_v4AudioProcessorEditor(Seq_v4AudioProcessor&);
    ~Seq_v4AudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:

    //==============================================================================

    void paintRythm(juce::Graphics& g);

    //==============================================================================

    void setSliderParams(juce::Slider& slider);
    void setNoteNumberComboBoxParams();

    //==============================================================================

    // Listener del comboBox de la nota musical
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

    //==============================================================================

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Seq_v4AudioProcessor& audioProcessor;

    // alias para que sea más legible
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    //using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    juce::Slider stepsSlider;
    juce::Slider eventsSlider;
    juce::Slider rotationSlider;
    juce::ComboBox noteNumberComboBox;
    unique_ptr<SliderAttachment> stepsSliderAttachment;
    unique_ptr<SliderAttachment> eventsSliderAttachment;
    unique_ptr<SliderAttachment> rotationSliderAttachment;
    //unique_ptr< ComboBoxAttachment> noteNumberComboBoxAttachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Seq_v4AudioProcessorEditor)
};
