#include "AudioProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
class SamplePlayer : public ProcessorBase
{
public:
    //...
    SamplePlayer()
        : ProcessorBase(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true))
    {
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

    void setSource(juce::AudioFormatReaderSource *newSource, double sampleRate)
    {
        transportSource.setSource(newSource, 0, nullptr, sampleRate);
    }

    void start() { transportSource.start(); }
    void stop() { transportSource.stop(); }
    bool test() { return transportSource.isPlaying(); }
    void setPosition(double newPosition) { transportSource.setPosition(newPosition); }

private:
    juce::AudioTransportSource transportSource;
};