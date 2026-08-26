#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

class HighpassNode : public ProcessorBase
{
public:
    virtual void setParameter(const juce::String &paramID, const juce::var &value) override
    {
        if (paramID == "setFrequency" || paramID == "setFrequencyHPF")
        {
            setFrequency(value);
            std::cout << "HPF freq adjusted to " << int(value) << std::endl;
        }
    }
    void setFrequency(float newFrequency)
    {
        cutoffFrequency = newFrequency;
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, cutoffFrequency);
    }
    HighpassNode()
    {
    }
    const juce::String getName() const override { return "highPassFilter"; }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 1000.0f);
        juce::dsp::ProcessSpec spec{sampleRate, static_cast<juce::uint32>(samplesPerBlock), 2};
        filter.prepare(spec);
    }
    void processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &) override
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        filter.process(context);
    }
    void reset() override
    {
        filter.reset();
    }

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
    float cutoffFrequency = 1000.0f;
    double sampleRate = 44100;
};