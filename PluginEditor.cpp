/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Seq_v4AudioProcessorEditor::Seq_v4AudioProcessorEditor(Seq_v4AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

    setSize(400, 300);

    // alias para que sea más legible
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    //using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    // memory allocation
    stepsSliderAttachment = make_unique<SliderAttachment>(audioProcessor.apvts, "STEPS", stepsSlider);
    eventsSliderAttachment = make_unique<SliderAttachment>(audioProcessor.apvts, "EVENTS", eventsSlider);
    rotationSliderAttachment = make_unique<SliderAttachment>(audioProcessor.apvts, "ROTATION", rotationSlider);
    //noteNumberComboBoxAttachment = make_unique<ComboBoxAttachment>(audioProcessor.apvts, "NOTE_NUMBER", noteNumberComboBox);

    setSliderParams(stepsSlider);
    setSliderParams(eventsSlider);
    setSliderParams(rotationSlider);

	noteNumberComboBox.addListener(this);
    setNoteNumberComboBoxParams();

    //noteNumberComboBox.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    //slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    //addAndMakeVisible(slider);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

}

Seq_v4AudioProcessorEditor::~Seq_v4AudioProcessorEditor()
{
}

//==============================================================================
void Seq_v4AudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);
}

void Seq_v4AudioProcessorEditor::resized()
{
    int numOfSliders = 4;

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
    rotationSlider.setBounds(eventsSlider.getRight() + padding, sliderStartY, sliderWidth, sliderHeight);
    noteNumberComboBox.setBounds(rotationSlider.getRight() + padding, sliderStartY, sliderWidth, sliderHeight);


}

//==============================================================================

void Seq_v4AudioProcessorEditor::setSliderParams(juce::Slider& slider) {
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
    addAndMakeVisible(slider);
}

void Seq_v4AudioProcessorEditor::setNoteNumberComboBoxParams() {
    
	noteNumberComboBox.addItem("C0", 24);
	noteNumberComboBox.addItem("C#0", 25);
	noteNumberComboBox.addItem("D0", 26);
	noteNumberComboBox.addItem("D#0", 27);
	noteNumberComboBox.addItem("E0", 28);
	noteNumberComboBox.addItem("F0", 29);
	noteNumberComboBox.addItem("F#0", 30);
	noteNumberComboBox.addItem("G0", 31);
	noteNumberComboBox.addItem("G#0", 32);
	noteNumberComboBox.addItem("A0", 33);
	noteNumberComboBox.addItem("A#0", 34);
	noteNumberComboBox.addItem("B0", 35);

	noteNumberComboBox.addItem("C1", 36);
	noteNumberComboBox.addItem("C#1", 37);
	noteNumberComboBox.addItem("D1", 38);
	noteNumberComboBox.addItem("D#1", 39);
	noteNumberComboBox.addItem("E1", 40);
	noteNumberComboBox.addItem("F1", 41);
	noteNumberComboBox.addItem("F#1", 42);
	noteNumberComboBox.addItem("G1", 43);
	noteNumberComboBox.addItem("G#1", 44);
	noteNumberComboBox.addItem("A1", 45);
	noteNumberComboBox.addItem("A#1", 46);
	noteNumberComboBox.addItem("B1", 47);

	noteNumberComboBox.addItem("C2", 48);
	noteNumberComboBox.addItem("C#2", 49);
	noteNumberComboBox.addItem("D2", 50);
	noteNumberComboBox.addItem("D#2", 51);
	noteNumberComboBox.addItem("E2", 52);
	noteNumberComboBox.addItem("F2", 53);
	noteNumberComboBox.addItem("F#2", 54);
	noteNumberComboBox.addItem("G2", 55);
	noteNumberComboBox.addItem("G#2", 56);
	noteNumberComboBox.addItem("A2", 57);
	noteNumberComboBox.addItem("A#2", 58);
	noteNumberComboBox.addItem("B2", 59);

	noteNumberComboBox.addItem("C3", 60);
	noteNumberComboBox.addItem("C#3", 61);
	noteNumberComboBox.addItem("D3", 62);
	noteNumberComboBox.addItem("D#3", 63);
	noteNumberComboBox.addItem("E3", 64);
	noteNumberComboBox.addItem("F3", 65);
	noteNumberComboBox.addItem("F#3", 66);
	noteNumberComboBox.addItem("G3", 67);
	noteNumberComboBox.addItem("G#3", 68);
	noteNumberComboBox.addItem("A3", 69);
	noteNumberComboBox.addItem("A#3", 70);
	noteNumberComboBox.addItem("B3", 71);

	noteNumberComboBox.addItem("C4", 72);
	noteNumberComboBox.addItem("C#4", 73);
	noteNumberComboBox.addItem("D4", 74);
	noteNumberComboBox.addItem("D#4", 75);
	noteNumberComboBox.addItem("E4", 76);
	noteNumberComboBox.addItem("F4", 77);
	noteNumberComboBox.addItem("F#4", 78);
	noteNumberComboBox.addItem("G4", 79);
	noteNumberComboBox.addItem("G#4", 80);
	noteNumberComboBox.addItem("A4", 81);
	noteNumberComboBox.addItem("A#4", 82);
	noteNumberComboBox.addItem("B4", 83);

	noteNumberComboBox.addItem("C5", 84);
	noteNumberComboBox.addItem("C#5", 85);
	noteNumberComboBox.addItem("D5", 86);
	noteNumberComboBox.addItem("D#5", 87);
	noteNumberComboBox.addItem("E5", 88);
	noteNumberComboBox.addItem("F5", 89);
	noteNumberComboBox.addItem("F#5", 90);
	noteNumberComboBox.addItem("G5", 91);
	noteNumberComboBox.addItem("G#5", 92);
	noteNumberComboBox.addItem("A5", 93);
	noteNumberComboBox.addItem("A#5", 94);
	noteNumberComboBox.addItem("B5", 95);

	noteNumberComboBox.addItem("C6", 96);
	noteNumberComboBox.addItem("C#6", 97);
	noteNumberComboBox.addItem("D6", 98);
	noteNumberComboBox.addItem("D#6", 99);
	noteNumberComboBox.addItem("E6", 100);
	noteNumberComboBox.addItem("F6", 101);
	noteNumberComboBox.addItem("F#6", 102);
	noteNumberComboBox.addItem("G6", 103);
	noteNumberComboBox.addItem("G#6", 104);
	noteNumberComboBox.addItem("A6", 105);
	noteNumberComboBox.addItem("A#6", 106);
	noteNumberComboBox.addItem("B6", 107);

	noteNumberComboBox.setSelectedId(72); //C4
    noteNumberComboBox.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(noteNumberComboBox);
}

//==============================================================================

void Seq_v4AudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {
	
	audioProcessor.setNoteNumber(comboBoxThatHasChanged->getSelectedId());
}

//==============================================================================
