#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>
class GainControl : public ProcessorBase
{
public:
    virtual void setParameter(const juce::String &paramID, const juce::var &value) override
    {
        if (paramID == "adjustGain")
        {
            std::cout << "gain adjusted to " << int(value) << std::endl;
            adjustGain(value);
        }
    }

    GainControl()
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
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        gain.process(context);
    }
    void reset() override
    {
        gain.reset();
    }

private:
    juce::dsp::Gain<float> gain;
};