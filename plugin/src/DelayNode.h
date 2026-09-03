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
            updateBaseDelay();
            updateModulationDepth();
        }
        else if (paramID == "time")
        {
            lastTimeValue = (float)value;
            updateBaseDelay();
        }
        else if (paramID == "modulationDepth")
        {
            lastModDepthValue = (float)value;
            updateModulationDepth();
        }
        else if (paramID == "feedback")
        {
            feedbackAmt = juce::jlimit(0.0f, 0.999f, (float)value);
        }
        else if (paramID == "mix")
        {
            mixAmt = juce::jlimit(0.0f, 1.0f, (float)value);
        }
    }

    void prepareToPlay(double sr, int samplesPerBlock) override
    {
        sampleRate = sr;

        delayLine.setMaximumDelayInSamples((int)(2.0 * sampleRate));

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
                int modulated = juce::jlimit(0.0f, maxDelay,
                                             baseDelaySamples + modData[i] * modDepthSamples);
                std::cout << modulated << std::endl;

                delayLine.setDelay(modulated);
            }

            for (int ch = 0; ch < mainBuffer.getNumChannels(); ++ch)
            {
                auto *data = mainBuffer.getWritePointer(ch);
                const float input = data[i];
                const float delayed = delayLine.popSample(ch);

                data[i] = input + (delayed - input) * mixAmt;
                delayLine.pushSample(ch, input + delayed * feedbackAmt);
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

    void updateModulationDepth()
    {
        modDepthSamples = samplesToggle
                              ? lastModDepthValue
                              : (lastModDepthValue * (float)sampleRate) / 1000.0f;

        modDepthSamples = juce::jmax(0.0f, modDepthSamples);
    }

    bool samplesToggle = false;
    float lastTimeValue = 0.0f;
    float lastModDepthValue = 0.0f;
    float baseDelaySamples = 0.0f;
    float modDepthSamples = 0.0f;
    float mixAmt = 0.0f;
    double sampleRate = 44100.0;
    float feedbackAmt = 0.0f;
    juce::dsp::DelayLine<float> delayLine;
};