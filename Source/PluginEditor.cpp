#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================

YuayStutterAudioProcessorEditor::YuayStutterAudioProcessorEditor(
    YuayStutterAudioProcessor& p
)
    : AudioProcessorEditor(&p),
      audioProcessor(p)
{
    // ==========================
    // EDITOR SIZE
    // ==========================

    setSize(
        718,
        506
    );

    // ==========================
    // BACKGROUND
    // ==========================

    backgroundImage =
        juce::ImageCache::getFromMemory(
            BinaryData::background_png,
            BinaryData::background_pngSize
        );
        backgroundImage2 =
    juce::ImageCache::getFromMemory(
        BinaryData::background2_png,
        BinaryData::background2_pngSize
    );

// ==========================
// BYPASS
// ==========================

// ==========================
// BYPASS
// ==========================

bypassButton.setButtonText("");

bypassButton.setColour(
    juce::TextButton::buttonColourId,
    juce::Colours::transparentBlack
);

bypassButton.setColour(
    juce::TextButton::buttonOnColourId,
    juce::Colours::transparentBlack
);

bypassButton.setColour(
    juce::TextButton::textColourOffId,
    juce::Colours::transparentBlack
);

bypassButton.setColour(
    juce::TextButton::textColourOnId,
    juce::Colours::transparentBlack
);

// ==========================
// BYPASS
// ==========================

bypassButton.setClickingTogglesState(true);
bypassButton.onClick =
    [this]()
{
    if (auto* param =
        audioProcessor.apvts.getParameter(
            YuayParameterID::Bypass
        ))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(
            bypassButton.getToggleState() ? 1.0f : 0.0f
        );
        param->endChangeGesture();
    }

    repaint();
};

addAndMakeVisible(
    bypassButton
);
    // ==========================
    // KNOB 1 - STUTTER
    // ==========================

    addAndMakeVisible(
        stutterKnob
    );

    stutterKnob.setNumFrames(
        127
    );

    stutterKnob.setMaxFrame(
        85
    );

    stutterKnob.setScale(
        0.30f
    );

    stutterKnob.setReverseFrames(
        false
    );

    stutterKnob.setValue(
        0.0f,
        false
    );

    // ==========================
    // STUTTER VALUE
    // ==========================

    addAndMakeVisible(
        stutterValueLabel
    );

    stutterValueLabel.setText(
        "0",
        juce::dontSendNotification
    );

    stutterValueLabel.setJustificationType(
        juce::Justification::centred
    );

    stutterValueLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white
    );

    stutterValueLabel.setColour(
        juce::Label::backgroundColourId,
        juce::Colours::transparentBlack
    );

    // ==========================
    // EMBEDDED FONT
    // ==========================

    auto typeface =
        juce::Typeface::createSystemTypefaceFor(
            BinaryData::JSThanapornNormal_ttf,
            BinaryData::JSThanapornNormal_ttfSize
        );

    stutterValueLabel.setFont(
        juce::Font(
            juce::FontOptions(typeface)
                .withHeight(30.0f)
        )
    );

    // ==========================
    // KNOB 2 - REFRESH MODE
    // ==========================

    auto knob2Image =
        juce::ImageCache::getFromMemory(
            BinaryData::knob2_png,
            BinaryData::knob2_pngSize
        );

    modeKnob.setImage(
        knob2Image
    );

    modeKnob.setNumFrames(
        2
    );

    modeKnob.setScale(
        1.0f
    );

    addAndMakeVisible(
        modeKnob
    );

    modeKnob.setValue(
        0.0f,
        false
    );

    // ==========================
    // KNOB 3 - OUTPUT LEVEL
    // ==========================

    auto knob3Image =
        juce::ImageCache::getFromMemory(
            BinaryData::knob3_png,
            BinaryData::knob3_pngSize
        );

    outputLevelKnob.setImage(
        knob3Image
    );

    outputLevelKnob.setNumFrames(
        127
    );

    outputLevelKnob.setMaxFrame(
        126
    );

    // ขนาด knob จริง
    outputLevelKnob.setScale(
        0.13f
    );

    // strip ถูกสร้างมาในทิศถูกแล้ว
    outputLevelKnob.setReverseFrames(
        false
    );

    addAndMakeVisible(
        outputLevelKnob
    );

    // ==========================
    // OUTPUT LEVEL DEFAULT
    //
    // New instance display: unity gain.
    // ==========================

    outputLevelKnob.setValue(
        juce::jmap(
            0.0f,
            -60.0f,
            12.0f,
            0.0f,
            1.0f
        ),
        false
    );

    // ==========================
    // OUTPUT LEVEL VALUE
    // ==========================

    addAndMakeVisible(
        outputLevelLabel
    );

    outputLevelLabel.setJustificationType(
        juce::Justification::centred
    );

    outputLevelLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white
    );

    outputLevelLabel.setColour(
        juce::Label::backgroundColourId,
        juce::Colours::transparentBlack
    );

    outputLevelLabel.setFont(
        juce::Font(
            juce::FontOptions(typeface)
                .withHeight(22.0f)
        )
    );

    // ==========================
    // KNOB 4 - DRY / WET
    // ==========================

    auto knob4Image =
        juce::ImageCache::getFromMemory(
            BinaryData::knob4_png,
            BinaryData::knob4_pngSize
        );

    dryWetKnob.setImage(
        knob4Image
    );

    dryWetKnob.setNumFrames(
        127
    );

    dryWetKnob.setMaxFrame(
        126
    );

    // ขนาด knob จริง
    dryWetKnob.setScale(
        0.13f
    );

    // strip ถูกสร้างมาในทิศถูกแล้ว
    dryWetKnob.setReverseFrames(
        false
    );

    addAndMakeVisible(
        dryWetKnob
    );

    // ==========================
    // DRY / WET VALUE
    // ==========================

    addAndMakeVisible(
        dryWetLabel
    );

    dryWetLabel.setJustificationType(
        juce::Justification::centred
    );

    dryWetLabel.setColour(
        juce::Label::textColourId,
        juce::Colours::white
    );

    dryWetLabel.setColour(
        juce::Label::backgroundColourId,
        juce::Colours::transparentBlack
    );

    dryWetLabel.setFont(
        juce::Font(
            juce::FontOptions(typeface)
                .withHeight(22.0f)
        )
    );

    // ==========================
    // KNOB 1 CONTROL
    // ==========================

    stutterKnob.onValueChange =
        [this](float value)
    {
        if (auto* param =
            audioProcessor.apvts.getParameter(
                YuayParameterID::Stutter
            ))
        {
            param->setValueNotifyingHost(
                value
            );
        }
    };

    // ==========================
    // KNOB 2 CONTROL
    // ==========================

    modeKnob.onValueChange =
        [this](float value)
    {
        if (auto* param =
            audioProcessor.apvts.getParameter(
                YuayParameterID::RefreshMode
            ))
        {
            param->setValueNotifyingHost(
                value >= 0.5f
                    ? 1.0f
                    : 0.0f
            );
        }
    };

    // ==========================
    // KNOB 3 CONTROL
    // OUTPUT LEVEL
    // ==========================

    outputLevelKnob.onValueChange =
        [this](float value)
    {
        const float outputDb =
            juce::jmap(
                value,
                0.0f,
                1.0f,
                -60.0f,
                12.0f
            );

        if (auto* param =
            audioProcessor.apvts.getParameter(
                YuayParameterID::OutputLevel
            ))
        {
            param->setValueNotifyingHost(
                param->convertTo0to1(
                    outputDb
                )
            );
        }
    };

    // ==========================
    // KNOB 4 CONTROL
    // DRY / WET
    // ==========================

    if (auto* dryWetParam =
        audioProcessor.apvts.getRawParameterValue(
            YuayParameterID::DryWet
        ))
    {
        dryWetKnob.setValue(
            dryWetParam->load(),
            false
        );
    }

    dryWetKnob.onValueChange =
        [this](float value)
    {
        if (auto* param =
            audioProcessor.apvts.getParameter(
                YuayParameterID::DryWet
            ))
        {
            param->setValueNotifyingHost(
                value
            );
        }
    };

    // ==========================
    // TIMER
    // ==========================

    // The mode knob's transparent rectangle overlaps the edge of BYPASS.
    // Keep the existing invisible button above that rectangle for hit testing.
    bypassButton.toFront(false);
    tipButton.setTooltip("Support Yuaylooper on Ko-fi (optional)");
    auto tipImage = juce::ImageCache::getFromMemory(BinaryData::tipkofi_png, BinaryData::tipkofi_pngSize)
        .getClippedImage({65, 165, 1920, 455});
    tipButton.setImages(false, true, true, tipImage, 1.0f, juce::Colours::transparentBlack,
        tipImage, 0.9f, juce::Colours::transparentBlack,
        tipImage, 0.75f, juce::Colours::transparentBlack);
    tipButton.onClick = [] { juce::URL("https://ko-fi.com/yuaylooper").launchInDefaultBrowser(); };
    addAndMakeVisible(tipButton);
    resized();
    timerCallback();
    startTimerHz(
        30
    );
}

//==============================================================================

YuayStutterAudioProcessorEditor::~YuayStutterAudioProcessorEditor()
{
    removeMouseListener(this);
}

//==============================================================================
// PAINT
//==============================================================================

void YuayStutterAudioProcessorEditor::paint(
    juce::Graphics& g
)
{
    auto* bypassParameter =
    audioProcessor.apvts.getRawParameterValue(
        YuayParameterID::Bypass
    );

const bool bypassState =
    bypassParameter != nullptr
        ? bypassParameter->load() > 0.5f
        : false;

const auto& image =
    bypassState
        ? backgroundImage2
        : backgroundImage;
    g.drawImage(
        image,
        getLocalBounds().toFloat()
    );
}

//==============================================================================
// RESIZED
//==============================================================================

void YuayStutterAudioProcessorEditor::resized()
{
    tipButton.setBounds(getWidth() - 198, 45, 184, 44);

    // ==========================
    // KNOB 1 - STUTTER
    // ==========================

    stutterKnob.setBounds(
        50,
        90,
        280,
        280
    );

    // ==========================
    // STUTTER VALUE
    // ==========================

    stutterValueLabel.setBounds(
        151,
        372,
        74,
        32
    );

    // ==========================
    // KNOB 2 - REFRESH MODE
    //
    // ขยับขึ้น
    // ไม่ให้แมลงทับเส้นล่าง
    // ==========================

    modeKnob.setBounds(
        376,
        320,
        160,
        160
    );

    // ==========================
    // KNOB 3 - OUTPUT LEVEL
    //
    // จัดให้อยู่กลางวง
    // ==========================

    outputLevelKnob.setBounds(
        534,
        160,
        180,
        180
    );

    // ==========================
    // OUTPUT LEVEL VALUE
    // ==========================

    outputLevelLabel.setBounds(
        590,
        193,
        70,
        24
    );

    // ==========================
    // KNOB 4 - DRY / WET
    //
    // จัดให้อยู่กลางวง
    // ==========================

    dryWetKnob.setBounds(
        534,
        292,
        180,
        180
    );

    // ==========================
    // DRY / WET VALUE
    // ==========================

    dryWetLabel.setBounds(
        590,
        328,
        70,
        24
    );
   bypassButton.setBounds(
    330,
    455,
    58,
    42
);
}

//==============================================================================
// TIMER
//==============================================================================

void YuayStutterAudioProcessorEditor::timerCallback()
{
    const bool bypassed = audioProcessor.apvts.getRawParameterValue(YuayParameterID::Bypass)->load() > 0.5f;
    if (bypassButton.getToggleState() != bypassed)
    {
        bypassButton.setToggleState(bypassed, juce::dontSendNotification);
        repaint();
    }
    // ==========================
    // STUTTER
    // ==========================

    auto* stutterParameter =
        audioProcessor.apvts
            .getRawParameterValue(
                YuayParameterID::Stutter
            );

    if (stutterParameter != nullptr)
    {
        const float stutterValue =
            stutterParameter->load();

        if (!stutterKnob.isCurrentlyDragging())
        {
            stutterKnob.setValue(
                stutterValue,
                false
            );
        }

        const int displayValue =
            juce::roundToInt(
                stutterValue * 100.0f
            );

        stutterValueLabel.setText(
            juce::String(displayValue),
            juce::dontSendNotification
        );
    }

    // ==========================
    // OUTPUT LEVEL
    // ==========================

    auto* outputParameter =
        audioProcessor.apvts
            .getRawParameterValue(
                YuayParameterID::OutputLevel
            );

    if (outputParameter != nullptr)
    {
        const float outputValue =
            outputParameter->load();

        if (!outputLevelKnob.isCurrentlyDragging())
        {
            const float normalised =
                juce::jmap(
                    outputValue,
                    -60.0f,
                    12.0f,
                    0.0f,
                    1.0f
                );

            outputLevelKnob.setValue(
                normalised,
                false
            );
        }

        outputLevelLabel.setText(
            juce::String(
                outputValue,
                1
            ) + " dB",
            juce::dontSendNotification
        );
    }

    // ==========================
    // DRY / WET
    // ==========================

    auto* dryWetParameter =
        audioProcessor.apvts
            .getRawParameterValue(
                YuayParameterID::DryWet
            );

    if (dryWetParameter != nullptr)
    {
        const float dryWetValue =
            dryWetParameter->load();

        if (!dryWetKnob.isCurrentlyDragging())
        {
            dryWetKnob.setValue(
                dryWetValue,
                false
            );
        }

        const int dryWetPercent =
            juce::roundToInt(
                dryWetValue * 100.0f
            );

        dryWetLabel.setText(
            juce::String(
                dryWetPercent
            ) + "%",
            juce::dontSendNotification
        );
    }

    // ==========================
    // KNOB 2
    // ==========================

    auto* refreshParameter =
        audioProcessor.apvts
            .getRawParameterValue(
                YuayParameterID::RefreshMode
            );

    if (refreshParameter != nullptr)
    {
        const float refreshValue =
            refreshParameter->load();

        modeKnob.setValue(
            refreshValue,
            false
        );
    }
}

