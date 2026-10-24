#pragma once

#include <JuceHeader.h>
#include "filmstripknob.h"

class YuayStutterAudioProcessor;

class RecoveredPruedEditor
    : public juce::AudioProcessorEditor,
      private juce::Timer
{
public:

    RecoveredPruedEditor(
        YuayStutterAudioProcessor&
    );

    ~RecoveredPruedEditor() override;

    void paint(
        juce::Graphics&
    ) override;

    void resized() override;

    void timerCallback() override;

private:

    YuayStutterAudioProcessor& audioProcessor;

    juce::Image backgroundImage;
juce::Image backgroundImage2;

class InvisibleButton : public juce::Button
{
public:
    InvisibleButton()
        : juce::Button("InvisibleButton")
    {
    }

    void paintButton(
        juce::Graphics&,
        bool,
        bool
    ) override
    {
        // ไม่วาดอะไรเลย
    }
};

InvisibleButton bypassButton;

    // ==========================
    // KNOB 1
    // ==========================

    FilmStripKnob stutterKnob;


    // ==========================
    // KNOB 2
    // ==========================

    FilmStripKnob modeKnob;


    // ==========================
    // KNOB 3
    // OUTPUT LEVEL
    // ==========================

    FilmStripKnob outputLevelKnob;

    juce::Label outputLevelLabel;


    // ==========================
    // KNOB 4
    // DRY / WET
    // ==========================

    FilmStripKnob dryWetKnob;

    juce::Label dryWetLabel;


    // ==========================
    // STUTTER VALUE
    // ==========================

    juce::Label stutterValueLabel;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        RecoveredPruedEditor
    )
};