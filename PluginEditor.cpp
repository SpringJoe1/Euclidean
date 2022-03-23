/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Seq_v4AudioProcessorEditor::Seq_v4AudioProcessorEditor (Seq_v4AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
    setSize (400, 300);
    
    // alias para que sea más legible
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    // memory allocation
    stepsSliderAttachment = make_unique<SliderAttachment>(audioProcessor.apvts, "STEPS", stepsSlider);
    eventsSliderAttachment = make_unique<SliderAttachment>(audioProcessor.apvts, "EVENTS", eventsSlider);

    setSliderParams(stepsSlider);
    setSliderParams(eventsSlider);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

}

Seq_v4AudioProcessorEditor::~Seq_v4AudioProcessorEditor()
{
}

//==============================================================================
void Seq_v4AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
}

void Seq_v4AudioProcessorEditor::resized()
{
    int numOfSliders = 2;

    // bounds of the whole pluggin
    const auto bounds = getLocalBounds().reduced(10);
    // distance between sliders
    const auto padding = 10;
    // ancho del slider
    const auto sliderWidth = bounds.getWidth() / numOfSliders - padding;
    // alto del slider
    const auto sliderHeight = bounds.getWidth() / numOfSliders - padding;
    // donde empieza el primer slider eje X
    const auto sliderStartX = 0;
    // donde empieza el primer slider eje Y
    const auto sliderStartY = bounds.getHeight() / 2 - (sliderHeight / 2);

    // now we set de bounds
    stepsSlider.setBounds(sliderStartX, sliderStartY, sliderWidth, sliderHeight);
    eventsSlider.setBounds(stepsSlider.getRight() + padding, sliderStartY, sliderWidth, sliderHeight);

}

//==============================================================================

void Seq_v4AudioProcessorEditor::setSliderParams (juce::Slider& slider) {
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(slider);
}
