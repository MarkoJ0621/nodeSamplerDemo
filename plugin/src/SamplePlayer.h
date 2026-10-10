#include "AudioProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <atomic>
class SamplePlayer : public ProcessorBase
{
public:
    SamplePlayer()
        : ProcessorBase(BusesProperties()
                            .withInput("Input", juce::AudioChannelSet::stereo())
                            .withInput("Modulation", juce::AudioChannelSet::mono())
                            .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    {
        formatManager.registerBasicFormats();
    }
    void setParameter(const juce::String &paramID, const juce::var &value) override
    {
        if (paramID == "loadSample")
        {
            loadSample(value.toString());
        }
        if (paramID == "loop")
        {
            transportSource.setLooping((bool)value);
            if (readerSource != nullptr)
                readerSource->setLooping((bool)value);
            isLooping = (bool)value;
        }
        if (paramID == "playbackSpeed")
        {
            playbackSpeed.store(juce::jlimit(0.01f, 4.0f, (float)value));
        }
        if (paramID == "modulationDepth")
        {
            modDepth.store((float)value);
        }
    }
    void triggerAction(const juce::String &actionID) override
    {
        if (actionID == "start")
        {
            transportSource.setPosition(0.0);
            start();
        }
        else if (actionID == "stop")
        {
            stop();
        }
    }
    void loadSample(const juce::String &filePath)
    {
        juce::File file(filePath);

        if (auto *reader = formatManager.createReaderFor(file))
        {
            auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            readerSource = std::move(newSource); // must happen after setSource, so the transport already points elsewhere before we take ownership here
            transportSource.setLooping(isLooping);
            readerSource->setLooping(isLooping);
            std::cout << "Loaded sample: " << file.getFullPathName() << '\n';
            return;
        }

        std::cerr << "Could not load audio file: " << file.getFullPathName()
                  << '\n';
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        transportSource.prepareToPlay(samplesPerBlock, sampleRate);
        resampler.prepareToPlay(samplesPerBlock, sampleRate);
        resampler.setResamplingRatio(playbackSpeed.load());
    }

    void releaseResources() override { transportSource.releaseResources(); }

    void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &) override
    {
        buffer.clear();
        if (readerSource == nullptr)
            return;

        const auto modBuffer = getBusBuffer(buffer, true, 1);
        auto ratio = playbackSpeed.load();
        if (modBuffer.getNumChannels() > 0 && modBuffer.getNumSamples() > 0)
        {
            const auto *modulation = modBuffer.getReadPointer(0);
            ratio += modulation[modBuffer.getNumSamples() - 1] * modDepth.load();
        }

        resampler.setResamplingRatio(juce::jlimit(0.01f, 4.0f, ratio));
        juce::AudioSourceChannelInfo info(&buffer, 0, buffer.getNumSamples());
        resampler.getNextAudioBlock(info);
    }

    void start() { transportSource.start(); }
    void stop() { transportSource.stop(); }
    bool test() { return transportSource.isPlaying(); }
    void setPosition(double newPosition) { transportSource.setPosition(newPosition); }

private:
    juce::AudioFormatManager formatManager;
    bool isLooping = false;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    juce::ResamplingAudioSource resampler{&transportSource, false, 2};
    std::atomic<float> playbackSpeed{1.0f};
    std::atomic<float> modDepth{0.0f};
};