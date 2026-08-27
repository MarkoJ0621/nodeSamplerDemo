#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

class GainControl : public ProcessorBase
{
public:
    virtual void setParameter(const juce::String &paramID, const juce::var &value) override
    {
        if (paramID == "gain")
        {
            std::cout << "gain adjusted to " << int(value) << std::endl;
            adjustGain(value);
        }
    }

    GainControl()
        : ProcessorBase(BusesProperties()
                            .withInput("Input", juce::AudioChannelSet::stereo())
                            .withInput("Modulation", juce::AudioChannelSet::mono())
                            .withOutput("Output", juce::AudioChannelSet::stereo()))
    {
        gain.setGainDecibels(-6.0f);
    }
    const juce::String getName() const override { return "Gain"; }
    void adjustGain(float Newgain)
    {
        gain.setGainDecibels(Newgain);
    }
    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec spec{sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2};
        gain.prepare(spec);
    }

    void processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &) override
    {
        auto mainBuffer = getBusBuffer(buffer, true, 0); // main stereo input/output, in-place
        auto modBuffer = getBusBuffer(buffer, true, 1);  // modulation input

        juce::dsp::AudioBlock<float> block(mainBuffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        gain.process(context);

        if (modBuffer.getNumChannels() == 0)
            return; // nothing connected to modulation input — skip

        auto *modData = modBuffer.getReadPointer(0);

        for (int ch = 0; ch < mainBuffer.getNumChannels(); ++ch)
        {
            auto *data = mainBuffer.getWritePointer(ch);
            for (int i = 0; i < mainBuffer.getNumSamples(); ++i)
                data[i] *= 1.0f + modData[i];
        }
    }
    void reset() override
    {
        gain.reset();
    }

private:
    juce::dsp::Gain<float> gain;
};