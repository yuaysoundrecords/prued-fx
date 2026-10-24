#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================

YuayStutterAudioProcessor::YuayStutterAudioProcessor()

#ifndef JucePlugin_PreferredChannelConfigurations

:
AudioProcessor(
    BusesProperties()

#if !JucePlugin_IsMidiEffect

#if !JucePlugin_IsSynth

    .withInput(
        "Input",
        juce::AudioChannelSet::stereo(),
        true
    )

#endif

    .withOutput(
        "Output",
        juce::AudioChannelSet::stereo(),
        true
    )

#endif

),
apvts(
    *this,
    nullptr,
    "PARAMETERS",
    createParameterLayout()
)

#else

:
apvts(
    *this,
    nullptr,
    "PARAMETERS",
    createParameterLayout()
)

#endif

{
    stutterParameter =
        apvts.getRawParameterValue(
            YuayParameterID::Stutter
        );

    refreshParameter =
        apvts.getRawParameterValue(
            YuayParameterID::RefreshMode
        );

    outputLevelParameter =
        apvts.getRawParameterValue(
            YuayParameterID::OutputLevel
        );

    dryWetParameter =
        apvts.getRawParameterValue(
            YuayParameterID::DryWet
        );
        bypassParameter =
    apvts.getRawParameterValue(
        YuayParameterID::Bypass
    );

if (auto* bypassParam =
    apvts.getParameter(
        YuayParameterID::Bypass
    ))
{
    bypassParam->setValueNotifyingHost(0.0f);
}
    
}


YuayStutterAudioProcessor::~YuayStutterAudioProcessor()
{
}


//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout
YuayStutterAudioProcessor::createParameterLayout()
{
    using namespace juce;

    std::vector<
        std::unique_ptr<RangedAudioParameter>
    > parameters;


    // ==========================
    // STUTTER
    // ==========================

    parameters.push_back(
        std::make_unique<AudioParameterFloat>(
            YuayParameterID::Stutter,
            "Stutter",
            NormalisableRange<float>(
                0.0f,
                1.0f
            ),
            0.0f
        )
    );
    parameters.push_back(
    std::make_unique<AudioParameterBool>(
        YuayParameterID::Bypass,
        "Bypass",
        false
    )
);


    // ==========================
    // REFRESH MODE
    // ==========================

    parameters.push_back(
        std::make_unique<AudioParameterBool>(
            YuayParameterID::RefreshMode,
            "Refresh Mode",
            false
        )
    );


    // ==========================
    // OUTPUT LEVEL
    // -60 dB -> +12 dB
    // Default = -12 dB
    // Step = 1 dB
    // ==========================

    parameters.push_back(
        std::make_unique<AudioParameterFloat>(
            YuayParameterID::OutputLevel,
            "Output Level",
            NormalisableRange<float>(
                -60.0f,
                12.0f,
                1.0f
            ),
            -12.0f
        )
    );


    // ==========================
    // DRY / WET
    // 0% -> 100%
    // Default = 100%
    // Step = 1%
    // ==========================

    parameters.push_back(
        std::make_unique<AudioParameterFloat>(
            YuayParameterID::DryWet,
            "Dry / Wet",
            NormalisableRange<float>(
                0.0f,
                1.0f,
                0.01f
            ),
            1.0f
        )
    );


    return {
        parameters.begin(),
        parameters.end()
    };
}


//==============================================================================

float YuayStutterAudioProcessor::getStutterAmount() const noexcept
{
    return stutterParameter != nullptr
        ? stutterParameter->load()
        : 0.0f;
}


bool YuayStutterAudioProcessor::isRefreshMode() const noexcept
{
    return refreshParameter != nullptr
        ? refreshParameter->load() > 0.5f
        : false;
}


//==============================================================================

void YuayStutterAudioProcessor::prepareToPlay(
    double sampleRate,
    int samplesPerBlock
)
{
    stutterEngine.prepare(
        sampleRate,
        samplesPerBlock,
        getTotalNumOutputChannels()
    );
}


void YuayStutterAudioProcessor::releaseResources()
{
    stutterEngine.reset();
}


//==============================================================================

void YuayStutterAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages
)
{
    const bool isBypassed =
    bypassParameter != nullptr
        ? bypassParameter->load() > 0.5f
        : false;

if (isBypassed)
{
    return;
}
    // ==========================
    // MIDI CONTROL
    // ==========================

    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();

        if (msg.isController())
        {
            const int cc =
                msg.getControllerNumber();

            const int value =
                msg.getControllerValue();


            // ======================
            // CC1 -> Stutter
            // ======================

            if (cc == StutterCC)
            {
                const float stutterValue =
                    value / 127.0f;

                if (stutterParameter != nullptr)
                {
                    stutterParameter->store(
                        stutterValue
                    );
                }
            }


            // ======================
            // CC7 -> Refresh
            // ======================

            if (cc == RefreshCC)
            {
                const bool refreshOn =
                    value > 63;

                if (refreshParameter != nullptr)
                {
                    refreshParameter->store(
                        refreshOn
                            ? 1.0f
                            : 0.0f
                    );
                }
            }
        }
    }


    // ==========================
    // BPM
    // ==========================

    if (auto* currentPlayHead = getPlayHead())
    {
        if (auto position = currentPlayHead->getPosition())
        {
            if (position->getBpm().hasValue())
            {
                stutterEngine.setTempo(
                    *position->getBpm()
                );
            }
        }
    }


    // ==========================
    // DENORMAL PROTECTION
    // ==========================

    juce::ScopedNoDenormals noDenormals;


    // ==========================
    // SAVE DRY SIGNAL
    // ==========================

    juce::AudioBuffer<float> dryBuffer;

    dryBuffer.makeCopyOf(
        buffer
    );


    // ==========================
    // STUTTER PROCESSING
    // ==========================

    stutterEngine.process(
        buffer,
        getStutterAmount(),
        isRefreshMode()
    );


    // ==========================
    // DRY / WET
    // ==========================

    const float dryWet =
        dryWetParameter != nullptr
            ? dryWetParameter->load()
            : 1.0f;

    const float dryAmount =
        1.0f - dryWet;


    for (int channel = 0;
         channel < buffer.getNumChannels();
         ++channel)
    {
        auto* wetData =
            buffer.getWritePointer(channel);

        const auto* dryData =
            dryBuffer.getReadPointer(channel);


        for (int sample = 0;
             sample < buffer.getNumSamples();
             ++sample)
        {
            wetData[sample] =
                (dryData[sample] * dryAmount)
                +
                (wetData[sample] * dryWet);
        }
    }


    // ==========================
    // OUTPUT LEVEL
    // ==========================

    const float outputDb =
        outputLevelParameter != nullptr
            ? outputLevelParameter->load()
            : -12.0f;


    const float outputGain =
        juce::Decibels::decibelsToGain(
            outputDb
        );


    buffer.applyGain(
        outputGain
    );
}


//==============================================================================

bool YuayStutterAudioProcessor::hasEditor() const
{
    return true;
}


juce::AudioProcessorEditor*
YuayStutterAudioProcessor::createEditor()
{
    return new YuayStutterAudioProcessorEditor(
        *this
    );
}


//==============================================================================

const juce::String
YuayStutterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}


bool YuayStutterAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}


bool YuayStutterAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}


bool YuayStutterAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}


double YuayStutterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}


//==============================================================================

int YuayStutterAudioProcessor::getNumPrograms()
{
    return 1;
}


int YuayStutterAudioProcessor::getCurrentProgram()
{
    return 0;
}


void YuayStutterAudioProcessor::setCurrentProgram(int)
{
}


const juce::String
YuayStutterAudioProcessor::getProgramName(int)
{
    return {};
}


void YuayStutterAudioProcessor::changeProgramName(
    int,
    const juce::String&
)
{
}


//==============================================================================

void YuayStutterAudioProcessor::getStateInformation(
    juce::MemoryBlock& destData
)
{
    auto state =
        apvts.copyState();


    std::unique_ptr<juce::XmlElement> xml(
        state.createXml()
    );


    if (xml != nullptr)
    {
        copyXmlToBinary(
            *xml,
            destData
        );
    }
}


void YuayStutterAudioProcessor::setStateInformation(
    const void* data,
    int sizeInBytes
)
{
    std::unique_ptr<juce::XmlElement> xml(
        getXmlFromBinary(
            data,
            sizeInBytes
        )
    );


    if (xml == nullptr)
        return;


    if (!xml->hasTagName(
        apvts.state.getType()
    ))
        return;


    apvts.replaceState(
        juce::ValueTree::fromXml(
            *xml
        )
    );
}


//==============================================================================

juce::AudioProcessor*
JUCE_CALLTYPE createPluginFilter()
{
    return new YuayStutterAudioProcessor();
}