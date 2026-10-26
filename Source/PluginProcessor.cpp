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
    intensityParameter = apvts.getRawParameterValue(YuayParameterID::Intensity);
    syncParameter = apvts.getRawParameterValue(YuayParameterID::BeatSync);
    beatsParameter = apvts.getRawParameterValue(YuayParameterID::LoopBeats);
    recaptureParameter = apvts.getRawParameterValue(YuayParameterID::Recapture);
    defaultState = apvts.copyState();
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
    // New instances start at unity gain (0 dB).
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
            0.0f
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


    // Append new IDs; preserve the order, ranges and defaults of all 1.0 parameters.
    parameters.push_back(std::make_unique<AudioParameterFloat>(YuayParameterID::Intensity, "Intensity", 0.0f, 1.0f, 1.0f));
    parameters.push_back(std::make_unique<AudioParameterBool>(YuayParameterID::BeatSync, "Beat Sync", true));
    parameters.push_back(std::make_unique<AudioParameterChoice>(YuayParameterID::LoopBeats, "Loop Length",
        StringArray { "1/16 beat", "1/8 beat", "1/4 beat", "1/2 beat", "1 beat", "2 beats", "4 beats" }, 4));
    parameters.push_back(std::make_unique<AudioParameterBool>(YuayParameterID::Recapture, "Recapture", false));
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
    maximumBlock = juce::jmax(1, samplesPerBlock);
    dryBuffer.setSize(getTotalNumOutputChannels(), maximumBlock);
    performanceLooper.prepare(sampleRate, getTotalNumOutputChannels());
    mixSmoother.reset(sampleRate, 0.005);
    gainSmoother.reset(sampleRate, 0.005);
    bypassSmoother.reset(sampleRate, 0.005);
    mixSmoother.setCurrentAndTargetValue(dryWetParameter->load());
    gainSmoother.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(outputLevelParameter->load()));
    bypassSmoother.setCurrentAndTargetValue(bypassParameter->load());
    previousRecapture = false;

}


void YuayStutterAudioProcessor::releaseResources()
{
    performanceLooper.reset();
}


//==============================================================================

void YuayStutterAudioProcessor::processBlock(
    juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages
)
{
    juce::ScopedNoDenormals noDenormals;
    const int samples = buffer.getNumSamples();
    for (int ch = getTotalNumInputChannels(); ch < getTotalNumOutputChannels(); ++ch)
        buffer.clear(ch, 0, samples);
    if (samples == 0) return;

    double bpm = 120.0, ppq = 0.0;
    bool validPpq = false, playing = false;
    if (auto* head = getPlayHead())
        if (auto position = head->getPosition())
        {
            if (auto value = position->getBpm()) bpm = *value;
            if (auto value = position->getPpqPosition()) { ppq = *value; validPpq = true; }
            playing = position->getIsPlaying();
        }
    if (!std::isfinite(bpm) || bpm < 20.0 || bpm > 999.0) bpm = 120.0;
    performanceLooper.setTransport(bpm, ppq, validPpq, playing);
    const bool recaptureNow = recaptureParameter->load() > 0.5f;
    if (recaptureNow && !previousRecapture) performanceLooper.requestRecapture();
    previousRecapture = recaptureNow;

    int position = 0;
    for (auto metadata : midiMessages)
    {
        const int eventPosition = juce::jlimit(position, samples, metadata.samplePosition);
        processRange(buffer, position, eventPosition - position);
        position = eventPosition;
        const auto message = metadata.getMessage();
        if (message.isController())
        {
            const int cc = message.getControllerNumber();
            const int value = message.getControllerValue();
            // Audio-thread values are mirrored by APVTS on its normal timer.
            if (cc == StutterCC) stutterParameter->store(value / 127.0f);
            if (cc == RefreshCC) refreshParameter->store(value > 63 ? 1.0f : 0.0f);
            if (cc == 16) intensityParameter->store(value / 127.0f);
            if (cc == 17) syncParameter->store(value > 63 ? 1.0f : 0.0f);
            if (cc == 18)
            {
                const bool down = value > 63;
                if (down && !midiRecaptureDown) performanceLooper.requestRecapture();
                midiRecaptureDown = down;
            }
        }
    }
    processRange(buffer, position, samples - position);
    midiMessages.clear();
}

void YuayStutterAudioProcessor::processRange(juce::AudioBuffer<float>& buffer, int start, int count)
{
    const float lengths[] = { 0.0625f, 0.125f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f };
    while (count > 0)
    {
        int n = juce::jmin(count, maximumBlock);
        const int channels = buffer.getNumChannels();
        for (int ch = 0; ch < channels; ++ch)
            dryBuffer.copyFrom(ch, 0, buffer, ch, start, n);
        // A view into the host buffer; pointer storage is inline for stereo.
        juce::AudioBuffer<float> view(buffer.getArrayOfWritePointers(), channels, start, n);
        // The existing Stutter control directly drives the upgraded loop engine.
        const float amount = getStutterAmount();
        // Stutter selects the repeat rate and engages/releases the loop. It must
        // not attenuate wet activation: DRY_WET controls the audible dry blend.
        performanceLooper.process(view, 0, n, amount, intensityParameter->load(),
            syncParameter->load() > 0.5f,
            lengths[juce::jlimit(0, 6, static_cast<int>(beatsParameter->load()))], isRefreshMode());
        mixSmoother.setTargetValue(dryWetParameter->load());
        gainSmoother.setTargetValue(juce::Decibels::decibelsToGain(outputLevelParameter->load()));
        bypassSmoother.setTargetValue(bypassParameter->load() > 0.5f ? 1.0f : 0.0f);
        for (int i = 0; i < n; ++i)
        {
            const float mix = mixSmoother.getNextValue();
            const float gain = gainSmoother.getNextValue();
            const float bypass = bypassSmoother.getNextValue();
            for (int ch = 0; ch < channels; ++ch)
            {
                const float dry = dryBuffer.getSample(ch, i);
                const float wet = view.getSample(ch, i);
                const float processed = (dry + mix * (wet - dry)) * gain;
                view.setSample(ch, i, processed + bypass * (dry - processed));
            }
        }
        start += n;
        count -= n;
    }
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


    auto incoming = juce::ValueTree::fromXml(*xml);
    // Missing IDs use factory defaults, even after a newer preset was loaded.
    for (const auto& child : defaultState)
        if (!incoming.getChildWithProperty("id", child.getProperty("id")).isValid())
            incoming.addChild(child.createCopy(), -1, nullptr);
    if (auto trigger = incoming.getChildWithProperty("id", YuayParameterID::Recapture); trigger.isValid())
        trigger.setProperty("value", 0.0f, nullptr);
    apvts.replaceState(incoming);
}


//==============================================================================

juce::AudioProcessor*
JUCE_CALLTYPE createPluginFilter()
{
    return new YuayStutterAudioProcessor();
}
