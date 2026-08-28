#include "PluginProcessor.h"
#include "PluginEditor.h"

using AudioGraphIOProcessor = juce::AudioProcessorGraph::AudioGraphIOProcessor;
using Node = juce::AudioProcessorGraph::Node;
//==============================================================================
namespace nodeSamplerWebview
{
    AudioPluginAudioProcessor::AudioPluginAudioProcessor()
        : AudioProcessor(BusesProperties()
                             .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
          mainProcessor(new juce::AudioProcessorGraph()),
          parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
    {
        formatManager.registerBasicFormats();
    }

    AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
    {
    }

    //==============================================================================
    juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"gain", 1},
            "Gain",
            juce::NormalisableRange<float>{0.0f, 1.0f},
            0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"freq", 1},
            "Frequency",
            juce::NormalisableRange<float>{0.0f, 5.0f},
            0.5f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{"amp", 1},
            "Amplitude",
            juce::NormalisableRange<float>{0.0f, 1.0f},
            0.5f));

        return {params.begin(), params.end()};
    }
    //==============================================================================
    const juce::String AudioPluginAudioProcessor::getName() const
    {
        return JucePlugin_Name;
    }

    bool AudioPluginAudioProcessor::acceptsMidi() const
    {
#if JucePlugin_WantsMidiInput
        return true;
#else
        return false;
#endif
    }

    bool AudioPluginAudioProcessor::producesMidi() const
    {
#if JucePlugin_ProducesMidiOutput
        return true;
#else
        return false;
#endif
    }

    bool AudioPluginAudioProcessor::isMidiEffect() const
    {
#if JucePlugin_IsMidiEffect
        return true;
#else
        return false;
#endif
    }

    double AudioPluginAudioProcessor::getTailLengthSeconds() const
    {
        return 0.0;
    }

    int AudioPluginAudioProcessor::getNumPrograms()
    {
        return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
                  // so this should be at least 1, even if you're not really implementing programs.
    }

    int AudioPluginAudioProcessor::getCurrentProgram()
    {
        return 0;
    }

    void AudioPluginAudioProcessor::setCurrentProgram(int index)
    {
        juce::ignoreUnused(index);
    }

    const juce::String AudioPluginAudioProcessor::getProgramName(int index)
    {
        juce::ignoreUnused(index);
        return {};
    }

    void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String &newName)
    {
        juce::ignoreUnused(index, newName);
    }

    //==============================================================================
    void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
    {
        // Use this method as the place to do any pre-playback
        // initialisation that you need..
        juce::ignoreUnused(sampleRate, samplesPerBlock);
        mainProcessor->setPlayConfigDetails(getMainBusNumInputChannels(),
                                            getMainBusNumOutputChannels(),
                                            sampleRate,
                                            samplesPerBlock);
        mainProcessor->prepareToPlay(sampleRate, samplesPerBlock);
        initialiseGraph();
    }

    void AudioPluginAudioProcessor::releaseResources()
    {
        mainProcessor->releaseResources();

        // When playback stops, you can use this as an opportunity to free up any
        // spare memory, etc.
    }

    bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
    {
#if JucePlugin_IsMidiEffect
        juce::ignoreUnused(layouts);
        return true;
#else
        // This is the place where you check if the layout is supported.
        // In this template code we only support mono or stereo.
        // Some plugin hosts, such as certain GarageBand versions, will only
        // load plugins that support stereo bus layouts.
        if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
            return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
        if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
            return false;
#endif

        return true;
#endif
    }

    void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                                 juce::MidiBuffer &midiMessages)
    {
        juce::ignoreUnused(midiMessages);
        juce::ScopedNoDenormals noDenormals;

        mainProcessor->processBlock(buffer, midiMessages);
    }
    void AudioPluginAudioProcessor::initialiseGraph()
    {
        mainProcessor->clear();
        audioInputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioInputNode));
        audioOutputNode = mainProcessor->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));
        auto playerNode = std::make_unique<SamplePlayer>();
        samplePlayer = playerNode.get();
        samplerNode = mainProcessor->addNode(std::move(playerNode));

        auto gainNodeObject = std::make_unique<GainControl>();
        gainControl = gainNodeObject.get();
        gainNode = mainProcessor->addNode(std::move(gainNodeObject));
        // connectAudioNodes();
        nodes.push_back(audioInputNode);
        nodes.push_back(audioOutputNode);
        nodes.push_back(samplerNode);
        nodes.push_back(gainNode);
    }

    void AudioPluginAudioProcessor::connectAudioNodes(int source, int target, int channel)
    {
        if (source <= 0 || target <= 0 || source > static_cast<int>(nodes.size()) || target > static_cast<int>(nodes.size()))
            return;

        auto sourceNode = nodes[source - 1];
        auto targetNode = nodes[target - 1];
        if (!sourceNode || !targetNode)
            return;

        if (channel == 0 || channel == 1)
        {
            for (int channel = 0; channel < 2; ++channel)
            {
                mainProcessor->addConnection({{sourceNode->nodeID, channel},
                                              {targetNode->nodeID, channel}});
            }
        }

        if (channel == 2)
        {
            mainProcessor->addConnection({{sourceNode->nodeID, 0},
                                          {targetNode->nodeID, channel}});
            return;
        }
    }

    void AudioPluginAudioProcessor::removeConnection(int source, int target, int channel)
    {
        auto sourceNode = nodes[source - 1];
        auto targetNode = nodes[target - 1];
        if (channel == 2)
        {
            mainProcessor->removeConnection({{sourceNode->nodeID, 0},
                                             {targetNode->nodeID, channel}});
            return;
        }
        else
        {
            for (int channel = 0; channel < 2; ++channel)
            {
                mainProcessor->removeConnection({{sourceNode->nodeID, channel},
                                                 {targetNode->nodeID, channel}});
            }
        }
    }

    void AudioPluginAudioProcessor::deleteNode(int id)
    {
        std::cout << id << std::endl;
        mainProcessor->removeNode(nodes[id - 1]);
        nodes.erase(nodes.begin() + id - 1);
    }

    //==============================================================================
    bool AudioPluginAudioProcessor::hasEditor() const
    {
        return true; // (change this to false if you choose to not supply an editor)
    }

    juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
    {
        return new AudioPluginAudioProcessorEditor(*this);
    }

    //==============================================================================
    void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
    {
        // You should use this method to store your parameters in the memory block.
        // You could do that either as raw data, or use the XML or ValueTree classes
        // as intermediaries to make it easy to save and load complex data.
        juce::ignoreUnused(destData);
    }

    void AudioPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
    {
        // You should use this method to restore your parameters from this memory block,
        // whose contents will have been created by the getStateInformation() call.
        juce::ignoreUnused(data, sizeInBytes);
    }
    void AudioPluginAudioProcessor::startPlayback(int id)
    {
        if (id <= 0 || id > static_cast<int>(nodes.size()))
            return;

        auto node = nodes[id - 1];
        if (!node)
            return;

        if (auto *processor = dynamic_cast<ProcessorBase *>(node->getProcessor()))
            processor->triggerAction("start");
    }
    void AudioPluginAudioProcessor::stopPlayback(int id)
    {
        if (id <= 0 || id > static_cast<int>(nodes.size()))
            return;

        auto node = nodes[id - 1];
        if (!node)
            return;

        if (auto *processor = dynamic_cast<ProcessorBase *>(node->getProcessor()))
            processor->triggerAction("stop");
    }

    void AudioPluginAudioProcessor::newGainNode()
    {
        std::cout << "new node!" << std::endl;
        auto gainNodeObject = std::make_unique<GainControl>();
        gainControl = gainNodeObject.get();
        gainNode = mainProcessor->addNode(std::move(gainNodeObject));
        nodes.push_back(gainNode);
        for (int i = 0; i < nodes.size(); i++)
        {
            std::cout << nodes[i] << std::endl;
        }
    }
    void AudioPluginAudioProcessor::newNode(const juce::String &type)
    {
        std::unique_ptr<juce::AudioProcessor> nodeObject;
        std::cout << "new node!" << std::endl;
        std::cout << type.toStdString() << std::endl;
        if (type == "gainNode")
        {
            std::cout << "new gain node created" << std::endl;
            nodeObject = std::make_unique<GainControl>();
        }
        else if (type == "lowpassNode")
        {
            std::cout << "here???" << std::endl;
            nodeObject = std::make_unique<LowpassNode>();
        }
        else if (type == "highpassNode")
        {
            nodeObject = std::make_unique<HighpassNode>();
        }
        else if (type == "samplerNode")
        {
            std::cout << "new sampler node" << std::endl;
            nodeObject = std::make_unique<SamplePlayer>();
        }
        else if (type == "lfoNode")
        {
            std::cout << "new sampler node" << std::endl;
            nodeObject = std::make_unique<LfoNode>();
        }
        auto control = nodeObject.get();
        juce::AudioProcessorGraph::Node::Ptr node = mainProcessor->addNode(std::move(nodeObject));
        nodes.push_back(node);
        for (int i = 0; i < nodes.size(); i++)
        {
            std::cout << nodes[i] << std::endl;
        }
    }

    void AudioPluginAudioProcessor::setParameter(float value, int id, const juce::String &paramID)
    {
        std::cout << id << std::endl;
        if (id <= 0 || id > static_cast<int>(nodes.size()))
            return;
        auto node = nodes[id - 1];
        if (!node)
            return;
        if (auto *processor = dynamic_cast<ProcessorBase *>(node->getProcessor()))
            processor->setParameter(paramID, value);
    }

    void AudioPluginAudioProcessor::loadSample(const juce::String &filePath, int id)
    {
        if (id <= 0 || id > static_cast<int>(nodes.size()))
            return;

        auto node = nodes[id - 1];
        if (!node)
            return;

        if (auto *processor = dynamic_cast<ProcessorBase *>(node->getProcessor()))
            processor->setParameter("loadSample", filePath);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new nodeSamplerWebview::AudioPluginAudioProcessor();
}