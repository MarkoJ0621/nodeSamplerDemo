class DelayNode : public ProcessorBase
{
public:
    DelayNode()
        : ProcessorBase(BusesProperties()
                            .withInput("Input", juce::AudioChannelSet::stereo())
                            .withInput("Modulation", juce::AudioChannelSet::mono())
                            .withOutput("Output", juce::AudioChannelSet::stereo()))
    {
    }

    const juce::String getName() const override { return "Delay"; }

    void setParameter(const juce::String &paramID, const juce::var &value) override
    {
        if (paramID == "samplesToggle")
        {
            samplesToggle = (bool)value;
            updateBaseDelay(); // reinterpret the same raw slider value under the new unit
        }
        else if (paramID == "time")
        {
            lastTimeValue = (float)value;
            updateBaseDelay();
        }
        else if (paramID == "modDepth")
        {
            modDepthSamples = (float)value; // depth expressed directly in samples
        }
    }

    void prepareToPlay(double sr, int samplesPerBlock) override
    {
        sampleRate = sr;

        // must happen BEFORE prepare() — this is what was missing
        delayLine.setMaximumDelayInSamples((int)(2.0 * sampleRate)); // supports up to 2s of delay

        juce::dsp::ProcessSpec spec{sampleRate, (juce::uint32)samplesPerBlock, 2};
        delayLine.prepare(spec);

        updateBaseDelay();
    }

    void processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &) override
    {
        auto mainBuffer = getBusBuffer(buffer, true, 0);
        auto modBuffer = getBusBuffer(buffer, true, 1);

        bool hasModulation = modBuffer.getNumChannels() > 0 && modDepthSamples > 0.0f;
        const float *modData = hasModulation ? modBuffer.getReadPointer(0) : nullptr;

        float maxDelay = (float)delayLine.getMaximumDelayInSamples();

        for (int i = 0; i < mainBuffer.getNumSamples(); ++i)
        {
            if (hasModulation)
            {
                float modulated = juce::jlimit(0.0f, maxDelay,
                                               baseDelaySamples + modData[i] * modDepthSamples);
                delayLine.setDelay(modulated);
            }

            for (int ch = 0; ch < mainBuffer.getNumChannels(); ++ch)
            {
                auto *data = mainBuffer.getWritePointer(ch);
                delayLine.pushSample(ch, data[i]);
                data[i] = delayLine.popSample(ch);
            }
        }
    }

    void reset() override
    {
        delayLine.reset();
    }

private:
    void updateBaseDelay()
    {
        baseDelaySamples = samplesToggle
                               ? lastTimeValue
                               : (lastTimeValue * (float)sampleRate) / 1000.0f;

        baseDelaySamples = juce::jlimit(0.0f, (float)delayLine.getMaximumDelayInSamples(), baseDelaySamples);
        delayLine.setDelay(baseDelaySamples);
    }

    bool samplesToggle = false;
    float lastTimeValue = 0.0f;
    float baseDelaySamples = 0.0f;
    float modDepthSamples = 0.0f;
    double sampleRate = 44100.0;
    juce::dsp::DelayLine<float> delayLine;
};