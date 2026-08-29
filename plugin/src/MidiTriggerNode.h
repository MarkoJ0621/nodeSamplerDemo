#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>
#include <iostream>

class MidiTriggerNode : public ProcessorBase
{
public:
    virtual void setParameter(const juce::String &paramID, const juce::var &value) override
    {
        if (paramID == "setMidi")
        {
            MIDInote = static_cast<int>(value);
        }
    }

    bool matchesNoteNumber(int noteNumber) const
    {
        return noteNumber == MIDInote;
    }

    MidiTriggerNode()
        : ProcessorBase(BusesProperties()
                            .withInput("Input", juce::AudioChannelSet::stereo())
                            .withInput("MIDI Input", juce::AudioChannelSet::disabled(), true))
    {
    }
    const juce::String getName() const override { return "MidiTrigger"; }
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
    }

    void processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &midiMessages) override
    {
        juce::ignoreUnused(buffer);

        for (const auto metadata : midiMessages)
        {
            const auto message = metadata.getMessage();
            if (message.isNoteOnOrOff())
            {
                std::cout << "MidiTrigger received MIDI note "
                          << message.getNoteNumber()
                          << " with velocity " << message.getVelocity()
                          << std::endl;
            }

            if (message.isNoteOnOrOff() && message.getNoteNumber() == MIDInote)
            {
                lastTriggered = message.isNoteOn();
            }
        }
    }
    void reset() override
    {
        lastTriggered = false;
    }

    bool lastTriggered = false;

private:
    int MIDInote = 0;
};