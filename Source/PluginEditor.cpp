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
    // เปิด plugin = -12 dB
    // ==========================

    outputLevelKnob.setValue(
        juce::jmap(
            -12.0f,
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
    addMouseListener(this,true);
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
    if(waitingForLicenseResult && !audioProcessor.licensing().isChecking())
    {
        waitingForLicenseResult=false;
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,"Prued FX License",audioProcessor.licensing().status());
    }
    if(!expiryNotified && audioProcessor.trialRemainingSeconds()<=0 && !audioProcessor.licensing().isUnlocked())
    {
        expiryNotified=true;
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,"Prued FX Trial",
            "The 15-minute trial has ended. Audio now passes through dry.\nRemove and reinsert the plugin to try again, or right-click and activate your Gumroad license.");
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

void YuayStutterAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    if(!event.mods.isPopupMenu() || licenseMenuOpen)return;
    licenseMenuOpen=true;
    juce::PopupMenu menu;
    const auto minutes=juce::jmax(0,static_cast<int>(std::ceil(audioProcessor.trialRemainingSeconds()/60.0)));
    menu.addItem(100,audioProcessor.licensing().isUnlocked()?"Licensed - unlimited use":"Free trial - "+juce::String(minutes)+" min remaining",false);
    menu.addSeparator();menu.addItem(1,"Activate / License status...");menu.addItem(2,"Buy a license on Gumroad...");
    juce::Component::SafePointer<YuayStutterAudioProcessorEditor> safe(this);
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this),[safe](int result)
    {
        if(!safe)return;
        safe->licenseMenuOpen=false;
        if(result==1)safe->showLicenseDialog();
        if(result==2)juce::URL(LicenseManager::purchaseUrl).launchInDefaultBrowser();
    });
}
void YuayStutterAudioProcessorEditor::showLicenseDialog()
{
    auto* dialog=new juce::AlertWindow("Prued FX License",audioProcessor.licensing().status()
        +"\nEnter the key from your Gumroad receipt. Verification sends this key to Gumroad over HTTPS.",juce::MessageBoxIconType::NoIcon);
    dialog->addTextEditor("licenseKey",{},"License key:",true);
    dialog->addButton("Activate",1);dialog->addButton("Cancel",0);
    juce::Component::SafePointer<YuayStutterAudioProcessorEditor> safe(this);
    juce::Component::SafePointer<juce::AlertWindow> box(dialog);
    dialog->enterModalState(true,juce::ModalCallbackFunction::create([safe,box](int result)
    {
        if(result==1 && safe && box)
        {
            safe->waitingForLicenseResult=safe->audioProcessor.licensing().activate(box->getTextEditorContents("licenseKey"));
            if(!safe->waitingForLicenseResult)
                juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,"Prued FX License",safe->audioProcessor.licensing().status());
        }
    }),true);
}
