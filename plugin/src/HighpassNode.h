class HighpassNode : public ProcessorBase
{
public:
    HighpassNode() : ProcessorBase(BusesProperties()
                                       .withInput("Input", juce::AudioChannelSet::stereo())
                                       .withInput("Modulation", juce::AudioChannelSet::mono())
                                       .withOutput("Output", juce::AudioChannelSet::stereo()))
    {
    }

    const juce::String getName() const override { return "highPassFilter"; }

    void setParameter(const juce::String &paramID, const juce::var &value) override
    {
        if (paramID == "frequencyHPF")
            setFrequency((float)value);
        else if (paramID == "modDepth")
            modDepthHz = (float)value;
    }

    void setFrequency(float newFrequency)
    {
        baseFrequency = newFrequency;
        updateCoefficients(baseFrequency);
    }

    void prepareToPlay(double sr, int samplesPerBlock) override
    {
        sampleRate = sr;
        updateCoefficients(baseFrequency);
        juce::dsp::ProcessSpec spec{sampleRate, (juce::uint32)samplesPerBlock, 2};
        filter.prepare(spec);
    }

    void processBlock(juce::AudioSampleBuffer &buffer, juce::MidiBuffer &) override
    {
        auto mainBuffer = getBusBuffer(buffer, true, 0);
        auto modBuffer = getBusBuffer(buffer, true, 1);

        if (modBuffer.getNumChannels() > 0 && modDepthHz > 0.0f)
        {
            float modValue = modBuffer.getReadPointer(0)[0];
            float modulatedFreq = juce::jlimit(20.0f, 20000.0f,
                                               baseFrequency + modValue * modDepthHz);
            updateCoefficients(modulatedFreq);
        }

        juce::dsp::AudioBlock<float> block(mainBuffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        filter.process(context);
    }

    void reset() override { filter.reset(); }

private:
    void updateCoefficients(float frequency)
    {
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, frequency);
    }

    float baseFrequency = 1000.0f;
    float modDepthHz = 10000.0f;
    double sampleRate = 44100.0;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> filter;
};