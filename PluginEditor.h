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
class Seq_v4AudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    Seq_v4AudioProcessorEditor (Seq_v4AudioProcessor&);
    ~Seq_v4AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    //==============================================================================

    void Seq_v4AudioProcessorEditor::setSliderParams(juce::Slider& slider);

    //==============================================================================

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Seq_v4AudioProcessor& audioProcessor;
    
    // alias para que sea más legible
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    juce::Slider stepsSlider;
    juce::Slider eventsSlider;
    unique_ptr<SliderAttachment> stepsSliderAttachment;
    unique_ptr<SliderAttachment> eventsSliderAttachment;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Seq_v4AudioProcessorEditor)
};
