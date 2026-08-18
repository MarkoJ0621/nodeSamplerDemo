#pragma once

#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>

//==============================================================================
namespace nodeSamplerWebview
{
    class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
    {
    public:
        explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
        ~AudioPluginAudioProcessorEditor() override;
        void timerCallback() override;
        //==============================================================================
        void resized() override;

    private:
        // This reference is provided as a quick way for your editor to
        // access the processor object that created it.
        using Resource = juce::WebBrowserComponent::Resource;
        std::optional<Resource> getResource(const juce::String &url);

        AudioPluginAudioProcessor &processorRef;

        juce::WebBrowserComponent webView;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
    };
}