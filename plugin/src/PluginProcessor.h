#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_devices/sources/juce_AudioTransportSource.h>
#include "SamplePlayer.h"
#include <vector>

using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
using Node = juce::AudioProcessorGraph::Node;

//==============================================================================
namespace nodeSamplerWebview
{
    class AudioPluginAudioProcessor final : public juce::AudioProcessor
    {
    public:
        //==============================================================================
        AudioPluginAudioProcessor();
        ~AudioPluginAudioProcessor() override;

        //==============================================================================
        void prepareToPlay(double sampleRate, int samplesPerBlock) override;
        void releaseResources() override;

        bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

        void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
        using AudioProcessor::processBlock;

        //==============================================================================
        juce::AudioProcessorEditor *createEditor() override;
        bool hasEditor() const override;

        //==============================================================================
        const juce::String getName() const override;

        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;
        double getTailLengthSeconds() const override;

        //==============================================================================
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram(int index) override;
        const juce::String getProgramName(int index) override;
        void changeProgramName(int index, const juce::String &newName) override;

        //==============================================================================
        void getStateInformation(juce::MemoryBlock &destData) override;
        void setStateInformation(const void *data, int sizeInBytes) override;
        juce::AudioProcessorValueTreeState parameters;
        void loadSample(const juce::String &filePath);
        void startPlayback();
        void stopPlayback();
        void newSampler(const juce::String &filePath);

    private:
        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
        juce::Random random;
        static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
        double lfoPhase = 0.0;
        juce::AudioFormatManager formatManager;
        std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
        juce::AudioTransportSource transportSource;
        void initialiseGraph();
        void connectAudioNodes();
        void updateGraph();

        std::unique_ptr<juce::AudioProcessorGraph> mainProcessor;

        using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
        juce::AudioProcessorGraph::Node::Ptr audioInputNode;
        juce::AudioProcessorGraph::Node::Ptr audioOutputNode;
        juce::AudioProcessorGraph::Node::Ptr samplerNode;
        SamplePlayer *samplePlayer = nullptr;
    };
}