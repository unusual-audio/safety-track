/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SafetyTrackAudioProcessorEditor::SafetyTrackAudioProcessorEditor (SafetyTrackAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), thresholdAttachment(*p.threshold, thresholdSlider)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 280);
    
    addAndMakeVisible (thresholdSlider);
    thresholdSlider.setTextValueSuffix (" dB");
    
    thresholdSlider.setColour(juce::Slider::ColourIds::backgroundColourId, grey);
    thresholdSlider.setColour(juce::Slider::ColourIds::thumbColourId, red);
    thresholdSlider.setColour(juce::Slider::ColourIds::trackColourId, grey);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxTextColourId, grey);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxOutlineColourId, black);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxHighlightColourId, red);
    thresholdSlider.setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, black);

    startTimer(20);
}

SafetyTrackAudioProcessorEditor::~SafetyTrackAudioProcessorEditor()
{

}

void SafetyTrackAudioProcessorEditor::drawBarGraph(juce::Graphics& g, int x, int y, int w, int minValue, int maxValue, float level, float target, juce::String valueLabel, juce::String warningLabel, juce::Colour colour)
{
    g.setColour (colour);
    g.setFont(18);
    g.drawFittedText(valueLabel, x, y, 110, 30, juce::Justification::right, 1);

    g.setColour (grey);
    g.drawRect(x + 120, y + 14, w - 120, 1);
    
    if (target <= maxValue) {
        int warning = (w - 120) + (w - 120) / (maxValue - minValue) * target - ((w - 120) / (maxValue - minValue) * maxValue);
        g.drawRect(x + 120 + warning, y, 1, 29);
        g.setFont(12);
        g.drawFittedText(warningLabel, x + 120 + warning - 40, y - 30, 80, 30, juce::Justification::centred, 1);
    }
    
    int amplitude = (w - 120) + (w - 120) / (maxValue - minValue) * level - ((w - 120) / (maxValue - minValue) * maxValue);
    if (amplitude > 0)
    {
        g.setColour (colour);
        g.fillRect(x + 120, y + 7, amplitude > (w - 120) ? (w - 120) : amplitude, 15);
    }
    
}

//==============================================================================
void SafetyTrackAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (black);
    g.setColour(white);
    g.setFont(18);
    g.drawFittedText("Safety Track", 40,  30, 320, 30, juce::Justification::left, 1);
    
    g.setColour(grey);
    g.drawFittedText("L", 40,  80, 320, 29, juce::Justification::left, 1);
    g.drawFittedText("R", 40, 109, 320, 29, juce::Justification::left, 1);

    bool actualClipping = fmin(audioProcessor.absL, audioProcessor.absR) >= 1.0;
    float warning = (float) *audioProcessor.threshold;
    float value;
    juce::String valueLabel;
    juce::Colour colour;
    value = juce::Decibels::gainToDecibels(audioProcessor.absL, -96.0f);
    valueLabel = juce::String::formatted("%.1f dB", value);
    valueLabel = value <= -96.0f ? "-inf dB" : valueLabel;
    colour = audioProcessor.activeChannel == 0 ? actualClipping ? red : green : grey;
    juce::String warningLabel = juce::String::formatted("%.1f dB", (float) *audioProcessor.threshold);
    drawBarGraph(g, 40,  80, 320, -60, +6, value, warning, valueLabel, warningLabel, colour);
    value = juce::Decibels::gainToDecibels(audioProcessor.absR, -96.0f);
    valueLabel = juce::String::formatted("%.1f dB", value);
    valueLabel = value <= -96.0f ? "-inf dB" : valueLabel;
    colour = audioProcessor.activeChannel == 1 ? actualClipping ? red : green : grey;
    drawBarGraph(g, 40, 109, 320, -60, +6, value, warning, valueLabel, "", colour);
    
    g.setFont(15);
    g.setColour(grey);
    g.drawFittedText("Threshold:", 40, 170, 80, 30, juce::Justification::left, 1);

    g.setFont(12);
    g.setColour(grey);
    g.drawFittedText("Unusual Audio", 40, getHeight() - 60, 320, 30, juce::Justification::left, 1);
}

void SafetyTrackAudioProcessorEditor::timerCallback()
{
    repaint();
}

void SafetyTrackAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    thresholdSlider.setBounds (120, 170, 240, 30);
}
