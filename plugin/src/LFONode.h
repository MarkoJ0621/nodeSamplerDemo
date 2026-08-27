class LfoNode : public ProcessorBase
{
public:
    LfoNode() : ProcessorBase(BusesProperties().withOutput("Output", juce::AudioChannelSet::mono(), true)) {}

    void setParameter(const juce::String &paramID, const juce::var &value) override
    {
        if (paramID == "lfofreq")
            rate = (float)value;
        else if (paramID == "lfoamp")
            amplitude = (float)value;
    }

    void prepareToPlay(double sr, int) override { sampleRate = sr; }
    void releaseResources() override {}

    void processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &) override
    {
        auto *data = buffer.getWritePointer(0);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            data[i] = amplitude * std::sin(2.0f * juce::MathConstants<float>::pi * phase);
            phase += rate / (float)sampleRate;
            if (phase >= 1.0f)
                phase -= 1.0f;
        }
    }

private:
    double sampleRate = 44100.0;
    float rate = 1.0f;
    float amplitude = 0.5f;
    float phase = 0.0f;
};