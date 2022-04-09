/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
EucSeq_MultiStageAudioProcessorEditor::EucSeq_MultiStageAudioProcessorEditor(EucSeq_MultiStageAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{


	// alias para que sea m�s legible
	using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

	for (int i = 0; i < NUM_TOTAL_ETAPAS; i++) {

		// on-off button
		onOffButtons.insert({ i, new juce::TextButton{"onOffButton"} });
		onOffButtons.at(i)->setButtonText("Off");
		setTextButtonParams(*onOffButtons.at(i), "ON_OFF_BUTTON" + to_string(i));

		// memory allocation
		stepsSliders.insert({ i, new juce::Slider()});
		stepsSliderAttachments.insert({ i, make_unique<SliderAttachment>(audioProcessor.apvts, "STEPS" + to_string(i), (*stepsSliders.at(i))) });
		
		eventsSliders.insert({ i, new juce::Slider()});
		eventsSliderAttachments.insert({ i, make_unique<SliderAttachment>(audioProcessor.apvts, "EVENTS" + to_string(i), (*eventsSliders.at(i))) });
		
		rotationSliders.insert({ i, new juce::Slider()});
		rotationSliderAttachments.insert({ i, make_unique<SliderAttachment>(audioProcessor.apvts, "ROTATION" + to_string(i), (*rotationSliders.at(i))) });
		
		velocitySliders.insert({ i, new juce::Slider()});
		velocitySliderAttachments.insert({ i, make_unique<SliderAttachment>(audioProcessor.apvts, "VELOCITY" + to_string(i), (*velocitySliders.at(i))) });
		
		gateSliders.insert({ i, new juce::Slider()});
		gateSliderAttachments.insert({ i, make_unique<SliderAttachment>(audioProcessor.apvts, "GATE" + to_string(i), (*gateSliders.at(i))) });


		setSliderParams(*stepsSliders.at(i));
		setSliderParams(*eventsSliders.at(i));
		setSliderParams(*rotationSliders.at(i));
		setSliderParams(*velocitySliders.at(i));
		setSliderParams(*gateSliders.at(i));

		noteNumberComboBoxes.insert({ i, new juce::ComboBox() });
		setNoteNumberComboBoxParams(*noteNumberComboBoxes.at(i), "NOTE_NUMBER_COMBOBOX" + to_string(i));

		stepDurationComboBoxes.insert({ i, new juce::ComboBox() });
		setDurationComboBoxParams(*stepDurationComboBoxes.at(i), "STEP_DURATION_COMBOBOX" + to_string(i));

		// direction button
		reverseButtons.insert({ i, new juce::TextButton{"reverseButton"} });
		reverseButtons.at(i)->setButtonText("Reverse\nOff");
		reverseButtons.at(i)->setEnabled(false);
		setTextButtonParams(*reverseButtons.at(i), "REVERSE_BUTTON" + to_string(i));
	}
	
	setSize(800, 500);
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.


}

EucSeq_MultiStageAudioProcessorEditor::~EucSeq_MultiStageAudioProcessorEditor()
{
}

//==============================================================================
void EucSeq_MultiStageAudioProcessorEditor::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(juce::Colours::black);
	//paintRhythm(g);
}

void EucSeq_MultiStageAudioProcessorEditor::resized()
{
	int numOfComponents = 9;

	// bounds of the whole pluggin
	const auto bounds = getLocalBounds();
	// distance between components
	const auto padding = 10;
	// ancho del slider
	const auto componentWidth = bounds.getWidth() / numOfComponents - padding;
	// alto del slider
	const auto componentHeight = bounds.getHeight() / NUM_TOTAL_ETAPAS - 2*padding;
	// donde empieza el primer componente eje X
	auto componentStartX = 0;
	// donde empieza el primer componente eje Y
	auto componentStartY = 0;

	for (int i = 0; i < NUM_TOTAL_ETAPAS; i++) {
		
		(*onOffButtons.at(i)).setBounds(componentStartX, componentStartY, componentWidth, componentHeight);
		(*stepsSliders.at(i)).setBounds((*onOffButtons.at(i)).getRight() + padding, componentStartY, componentWidth, componentHeight);
		(*eventsSliders.at(i)).setBounds((*stepsSliders.at(i)).getRight() + padding, componentStartY, componentWidth, componentHeight);
		(*rotationSliders.at(i)).setBounds((*eventsSliders.at(i)).getRight() + padding, componentStartY, componentWidth, componentHeight);
		(*velocitySliders.at(i)).setBounds((*rotationSliders.at(i)).getRight() + padding, componentStartY, componentWidth, componentHeight);
		(*gateSliders.at(i)).setBounds((*velocitySliders.at(i)).getRight() + padding, componentStartY, componentWidth, componentHeight);
		(*noteNumberComboBoxes.at(i)).setBounds((*gateSliders.at(i)).getRight() + padding, componentStartY, componentWidth, componentHeight);
		(*stepDurationComboBoxes.at(i)).setBounds((*noteNumberComboBoxes.at(i)).getRight() + padding, componentStartY, componentWidth, componentHeight);
		(*reverseButtons.at(i)).setBounds((*stepDurationComboBoxes.at(i)).getRight() + padding, componentStartY, componentWidth, componentHeight);

		componentStartY += componentHeight + padding;
	}

}

//==============================================================================
void EucSeq_MultiStageAudioProcessorEditor::setTextButtonParams(juce::TextButton& textButton, string id) {
	
	textButton.setClickingTogglesState(true);
	textButton.setComponentID(id);
	addAndMakeVisible(textButton);
	textButton.addListener(this);
}

void EucSeq_MultiStageAudioProcessorEditor::setSliderParams(juce::Slider& slider) {
	//slider.setDoubleClickReturnValue
	//slider.setNumDecimalPlacesToDisplay
	slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 25);
	addAndMakeVisible(slider);
	slider.setEnabled(false);
}

void EucSeq_MultiStageAudioProcessorEditor::setNoteNumberComboBoxParams(juce::ComboBox& comboBox, string id) {

	comboBox.setComponentID(id);
	
	comboBox.addItem("C0", 24);
	comboBox.addItem("C#0", 25);
	comboBox.addItem("D0", 26);
	comboBox.addItem("D#0", 27);
	comboBox.addItem("E0", 28);
	comboBox.addItem("F0", 29);
	comboBox.addItem("F#0", 30);
	comboBox.addItem("G0", 31);
	comboBox.addItem("G#0", 32);
	comboBox.addItem("A0", 33);
	comboBox.addItem("A#0", 34);
	comboBox.addItem("B0", 35);
	
	comboBox.addItem("C1", 36);
	comboBox.addItem("C#1", 37);
	comboBox.addItem("D1", 38);
	comboBox.addItem("D#1", 39);
	comboBox.addItem("E1", 40);
	comboBox.addItem("F1", 41);
	comboBox.addItem("F#1", 42);
	comboBox.addItem("G1", 43);
	comboBox.addItem("G#1", 44);
	comboBox.addItem("A1", 45);
	comboBox.addItem("A#1", 46);
	comboBox.addItem("B1", 47);
	
	comboBox.addItem("C2", 48);
	comboBox.addItem("C#2", 49);
	comboBox.addItem("D2", 50);
	comboBox.addItem("D#2", 51);
	comboBox.addItem("E2", 52);
	comboBox.addItem("F2", 53);
	comboBox.addItem("F#2", 54);
	comboBox.addItem("G2", 55);
	comboBox.addItem("G#2", 56);
	comboBox.addItem("A2", 57);
	comboBox.addItem("A#2", 58);
	comboBox.addItem("B2", 59);
	
	comboBox.addItem("C3", 60);
	comboBox.addItem("C#3", 61);
	comboBox.addItem("D3", 62);
	comboBox.addItem("D#3", 63);
	comboBox.addItem("E3", 64);
	comboBox.addItem("F3", 65);
	comboBox.addItem("F#3", 66);
	comboBox.addItem("G3", 67);
	comboBox.addItem("G#3", 68);
	comboBox.addItem("A3", 69);
	comboBox.addItem("A#3", 70);
	comboBox.addItem("B3", 71);
	
	comboBox.addItem("C4", 72);
	comboBox.addItem("C#4", 73);
	comboBox.addItem("D4", 74);
	comboBox.addItem("D#4", 75);
	comboBox.addItem("E4", 76);
	comboBox.addItem("F4", 77);
	comboBox.addItem("F#4", 78);
	comboBox.addItem("G4", 79);
	comboBox.addItem("G#4", 80);
	comboBox.addItem("A4", 81);
	comboBox.addItem("A#4", 82);
	comboBox.addItem("B4", 83);
	
	comboBox.addItem("C5", 84);
	comboBox.addItem("C#5", 85);
	comboBox.addItem("D5", 86);
	comboBox.addItem("D#5", 87);
	comboBox.addItem("E5", 88);
	comboBox.addItem("F5", 89);
	comboBox.addItem("F#5", 90);
	comboBox.addItem("G5", 91);
	comboBox.addItem("G#5", 92);
	comboBox.addItem("A5", 93);
	comboBox.addItem("A#5", 94);
	comboBox.addItem("B5", 95);
	
	comboBox.addItem("C6", 96);
	comboBox.addItem("C#6", 97);
	comboBox.addItem("D6", 98);
	comboBox.addItem("D#6", 99);
	comboBox.addItem("E6", 100);
	comboBox.addItem("F6", 101);
	comboBox.addItem("F#6", 102);
	comboBox.addItem("G6", 103);
	comboBox.addItem("G#6", 104);
	comboBox.addItem("A6", 105);
	comboBox.addItem("A#6", 106);
	comboBox.addItem("B6", 107);
	
	comboBox.setSelectedId(72); //C4
	comboBox.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(comboBox);
	comboBox.setEnabled(false);
	comboBox.addListener(this);
}

void EucSeq_MultiStageAudioProcessorEditor::setDurationComboBoxParams(juce::ComboBox& comboBox, string id) {

	comboBox.setComponentID(id);

	// we need the IDs so as we can not store floats,
	// we add the number x1000 so it is an int, and then we will divide again
	
	// 1 es redonda con 4 de duracion
	// 1/2 es blanca con 2 de duracion
	// 1/4 es negra con 1 de duracion
	// ...
	comboBox.addItem("1", 4 * CONST_DURATION_TIME_CONV);
	comboBox.addItem("1/2", 2 * CONST_DURATION_TIME_CONV);
	comboBox.addItem("1/4", 1 * CONST_DURATION_TIME_CONV);
	comboBox.addItem("1/8", 0.5 * CONST_DURATION_TIME_CONV);
	comboBox.addItem("1/16", 0.25 * CONST_DURATION_TIME_CONV);
	comboBox.addItem("1/32", 0.125 * CONST_DURATION_TIME_CONV);
	comboBox.addItem("1/64", 0.062 * CONST_DURATION_TIME_CONV);

	comboBox.setSelectedId(1 * CONST_DURATION_TIME_CONV); // negras 
	comboBox.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(comboBox);
	comboBox.setEnabled(false);
	comboBox.addListener(this);
}

void EucSeq_MultiStageAudioProcessorEditor::disableComponents(int id) {
	
	stepsSliders.at(id)->setEnabled(false);
	eventsSliders.at(id)->setEnabled(false);
	rotationSliders.at(id)->setEnabled(false);
	velocitySliders.at(id)->setEnabled(false);
	gateSliders.at(id)->setEnabled(false);
	noteNumberComboBoxes.at(id)->setEnabled(false);
	stepDurationComboBoxes.at(id)->setEnabled(false);
	reverseButtons.at(id)->setEnabled(false);
}

void EucSeq_MultiStageAudioProcessorEditor::enableComponents(int id) {

	stepsSliders.at(id)->setEnabled(true);
	eventsSliders.at(id)->setEnabled(true);
	rotationSliders.at(id)->setEnabled(true);
	velocitySliders.at(id)->setEnabled(true);
	gateSliders.at(id)->setEnabled(true);
	noteNumberComboBoxes.at(id)->setEnabled(true);
	stepDurationComboBoxes.at(id)->setEnabled(true);
	reverseButtons.at(id)->setEnabled(true);
}


//==============================================================================

// LISTENERS

void EucSeq_MultiStageAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) {

	DBG("!!!! COMBO BOX !!!");
	// string que almacena el ID del combobox + seqID
	juce::String componentID = comboBoxThatHasChanged->getComponentID();
	DBG("juce::String componentID " << componentID);
	// string que almacena el ID del combobox:
	// NOTE_NUMBER_COMBOBOX | NOTE_DURATION_COMBOBOX | STEP_DURATION_COMBOBOX
	juce::String componentIDWithoutID = componentID.dropLastCharacters(1);
	DBG("juce::String componentIDWithoutID " << componentIDWithoutID);
	
	// obtener seqID en formato Int: 0 | 1 | 2 | 3
	char componentIDLast = componentID.toStdString().back();
	DBG("char componentIDLast " << componentIDLast);
	int seqID = stoi(string(1, componentIDLast));
	DBG("int componentIDLastInt " << componentIDLast);

	// TODO -- necesario?

	if (seqID < 0 || seqID >= NUM_TOTAL_ETAPAS){
		return;
	}

	// si es el caso del comboboc de NOTE_NUMBER_COMBOBOX
	if (componentIDWithoutID == "NOTE_NUMBER_COMBOBOX")
		audioProcessor.setNewNoteNumber(comboBoxThatHasChanged->getSelectedId(), seqID);
	// Si es el caso del combobox de STEP_DURATION_COMBOBOX
	if (componentIDWithoutID == "STEP_DURATION_COMBOBOX")
		audioProcessor.setNewStepFigure((float)comboBoxThatHasChanged->getSelectedId() / CONST_DURATION_TIME_CONV, seqID);
}

void EucSeq_MultiStageAudioProcessorEditor::buttonClicked(juce::Button* button) {
	
	DBG("!!!! BUTTON !!!");
	// string que almacena el ID del combobox + seqID
	juce::String componentID = button->getComponentID();
	DBG("juce::String componentID " << componentID);
	// string que almacena el ID del combobox:
	// NOTE_NUMBER_COMBOBOX | NOTE_DURATION_COMBOBOX | STEP_DURATION_COMBOBOX
	juce::String componentIDWithoutID = componentID.dropLastCharacters(1);
	DBG("juce::String componentIDWithoutID " << componentIDWithoutID);
	// obtener seqID en formato Int: 0 | 1 | 2 | 3
	char componentIDLast = componentID.toStdString().back();
	DBG("char componentIDLast " << componentIDLast);
	int seqID = stoi(string(1, componentIDLast));
	DBG("int componentIDLastInt " << componentIDLast);


	// Si es el caso del combobox de NOTE_NUMBER_COMBOBOX
	if (componentIDWithoutID == "ON_OFF_BUTTON") {
		if (button->getToggleState() == false) {
			// change button text
			button->setButtonText("Off");
			// delete rythm
			audioProcessor.deleteRythm(seqID);
			// disable those components
			disableComponents(seqID);
		}
		else {
			// change button text
			button->setButtonText("On");

			// create rythm
			int steps = stepsSliders.at(seqID)->getValue();
			int events = eventsSliders.at(seqID)->getValue();
			int rotation = rotationSliders.at(seqID)->getValue();
			int velocity = velocitySliders.at(seqID)->getValue();
			int gate = gateSliders.at(seqID)->getValue();
			int noteNumber = noteNumberComboBoxes.at(seqID)->getSelectedId();
			float figureStep = stepDurationComboBoxes.at(seqID)->getSelectedId() / CONST_DURATION_TIME_CONV;
			bool reverse = reverseButtons.at(seqID)->getToggleState();

			audioProcessor.createRythm(seqID, steps, events, rotation, velocity, gate, noteNumber,
				figureStep, reverse);

			// disable those components
			enableComponents(seqID);
		}
	}
	else if (componentIDWithoutID == "REVERSE_BUTTON") {
		if (button->getToggleState() == true) {
			// reverse the direction of the rythm
			button->setButtonText("Reverse\nOn");
			audioProcessor.setNewDirection(seqID, true);
		}
		else {
			// set the right direction
			button->setButtonText("Reverse\nOff");
			audioProcessor.setNewDirection(seqID, false);
		}
	}
}

//==============================================================================

//void EucSeq_MultiStageAudioProcessorEditor::paintRhythm(juce::Graphics& g) {
//
//	// l�mites del plugin
//	const auto bounds = getLocalBounds().reduced(10);
//
//	// alto de los segmentos
//	const auto alturaSegmentos = 15;
//
//	// coordenadas (X,Y) donde empezar a pintar 
//	const auto startY = bounds.getBottom() - 5 - alturaSegmentos;
//	const auto startX = 5;
//
//	// numero de segmentos del ritmo
//	const auto numSeg = audioProcessor.getEuclideanRhythm().getSteps();
//	// anchura de cada segmento
//	const auto anchoSegmento = (bounds.getWidth() - 10) / numSeg;
//
//	DBG("numSeg " << numSeg);
//
//	for (int i = 0; i < numSeg; i++) {
//		if (audioProcessor.guarrada1) {
//			audioProcessor.guarrada1 = false;
//			g.setColour(juce::Colours::purple);
//			g.fillRect(startX + (anchoSegmento * i), startY, anchoSegmento, alturaSegmentos);
//		}
//		else {
//			audioProcessor.guarrada1 = true;
//			g.setColour(juce::Colours::rebeccapurple);
//			g.fillRect(startX + (anchoSegmento * i), startY, anchoSegmento, alturaSegmentos);
//		}
//	}
//}