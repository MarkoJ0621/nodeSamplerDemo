#include "AudioProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
class SamplePlayer : public ProcessorBase
{
public:
    SamplePlayer()
        : ProcessorBase(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
    {
        formatManager.registerBasicFormats();
    }
    void setParameter(const juce::String &paramID, const juce::var &value) override
    {
        if (paramID == "loadSample")
        {
            loadSample(value.toString());
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
        }
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        transportSource.prepareToPlay(samplesPerBlock, sampleRate);
    }

    void releaseResources() override { transportSource.releaseResources(); }

    void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &) override
    {
        juce::AudioSourceChannelInfo info(&buffer, 0, buffer.getNumSamples());
        transportSource.getNextAudioBlock(info);
    }

    void start() { transportSource.start(); }
    void stop() { transportSource.stop(); }
    bool test() { return transportSource.isPlaying(); }
    void setPosition(double newPosition) { transportSource.setPosition(newPosition); }

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
};