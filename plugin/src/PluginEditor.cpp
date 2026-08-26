#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
namespace nodeSamplerWebview
{
    namespace
    {
        std::vector<std::byte> streamToVector(juce::InputStream &stream)
        {
            using namespace juce;
            const auto sizeInBytes = static_cast<size_t>(stream.getTotalLength());
            std::vector<std::byte> result(sizeInBytes);
            stream.setPosition(0);
            [[maybe_unused]] const auto bytesRead =
                stream.read(result.data(), result.size());
            jassert(bytesRead == static_cast<ssize_t>(sizeInBytes));
            return result;
        }
    }
    static const char *getMimeForExtension(const juce::String &extension)
    {
        static const std::unordered_map<juce::String, const char *> mimeMap = {
            {{"htm"}, "text/html"},
            {{"html"}, "text/html"},
            {{"txt"}, "text/plain"},
            {{"jpg"}, "image/jpeg"},
            {{"jpeg"}, "image/jpeg"},
            {{"svg"}, "image/svg+xml"},
            {{"ico"}, "image/vnd.microsoft.icon"},
            {{"json"}, "application/json"},
            {{"png"}, "image/png"},
            {{"css"}, "text/css"},
            {{"map"}, "application/json"},
            {{"js"}, "text/javascript"},
            {{"woff2"}, "font/woff2"}};

        if (const auto it = mimeMap.find(extension.toLowerCase());
            it != mimeMap.end())
            return it->second;

        jassertfalse;
        return "";
    }
    AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
        : AudioProcessorEditor(&p), processorRef(p),
          webView(juce::WebBrowserComponent::Options{}.withResourceProvider([this](const auto &url)
                                                                            { return getResource(url); })
                      .withNativeIntegrationEnabled()
                      .withEventListener("gainChange", [this](const juce::var &value)
                                         {
        if (auto *param = processorRef.parameters.getParameter("gain"))
        {
            float normalized = param->convertTo0to1((float)value);
            param->setValueNotifyingHost(normalized);
        } })
                      .withEventListener("freqChange", [this](const juce::var &value)
                                         {
        if (auto *param = processorRef.parameters.getParameter("freq"))
        {
            float normalized = param->convertTo0to1((float)value);
            param->setValueNotifyingHost(normalized);
        } })
                      .withEventListener("ampChange", [this](const juce::var &value)
                                         {
        if (auto *param = processorRef.parameters.getParameter("amp"))
        {
            float normalized = param->convertTo0to1((float)value);
            param->setValueNotifyingHost(normalized);
        } })
                      .withNativeFunction("chooseFile", [this](const juce::Array<juce::var> &args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                                          {
    int id = args.size() > 0 ? (int) args[0] : -1;
    auto chooser = std::make_shared<juce::FileChooser>("Select an audio file", juce::File{}, "*.wav;*.aiff;*.mp3");

    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, completion, chooser, id](const juce::FileChooser &fc)
        {
            auto path = fc.getResult().getFullPathName();

            if (path.isNotEmpty())
                processorRef.loadSample(path, id);

            completion(juce::var(path));
        }); })
                      .withNativeFunction("start", [this](const juce::Array<juce::var> &args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                                          {
        processorRef.startPlayback(args[0]);
        completion("hello"); })
                      .withNativeFunction("stop", [this](const juce::Array<juce::var> &args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                                          {
        processorRef.stopPlayback(args[0]);
        completion("bye"); })
                      .withNativeFunction("addConnection", [this](const juce::Array<juce::var> &args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                                          {
        processorRef.connectAudioNodes(args[0], args[1]);
        completion("bye"); })
                      .withNativeFunction("removeConnection", [this](const juce::Array<juce::var> &args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                                          {
        processorRef.removeConnection(args[0], args[1]);
        completion("bye"); })
                      .withNativeFunction("adjustGain", [this](const juce::Array<juce::var> &args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                                          {
        processorRef.adjustGain(args[0],args[1]);
        completion("bye"); })
                      .withNativeFunction("deleteNode", [this](const juce::Array<juce::var> &args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                                          {
        processorRef.deleteNode(args[0]);
        completion("bye"); })
                      .withNativeFunction("newNode", [this](const juce::Array<juce::var> &args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                                          {
                                            std::cout<<args[0].toString()<<std::endl;
        processorRef.newNode(args[0].toString());
        completion("bye"); })
                      .withNativeFunction("setFreq", [this](const juce::Array<juce::var> &args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                                          {
                                            std::cout<<args[0].toString()<<std::endl;
        processorRef.setFrequency(args[0],args[1]);
        completion("bye"); })
                      .withNativeFunction("setFreqHPF", [this](const juce::Array<juce::var> &args, juce::WebBrowserComponent::NativeFunctionCompletion completion)
                                          {
                                            std::cout<<args[0].toString()<<std::endl;
        processorRef.setFrequencyHPF(args[0],args[1]);
        completion("bye"); }))
    {
        juce::ignoreUnused(processorRef);
        addAndMakeVisible(webView);
        setResizable(true, true);
        // Make sure that before the constructor has finished, you've set the
        // editor's size to whatever you need it to be.
        setSize(800, 600);
        webView.goToURL(webView.getResourceProviderRoot());
        startTimerHz(30);
    }

    AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
    {
        stopTimer();
    }
    void AudioPluginAudioProcessorEditor::timerCallback()
    {
    }
    void AudioPluginAudioProcessorEditor::resized()
    {
        webView.setBounds(getLocalBounds());
        // This is generally where you'll want to lay out the positions of any
        // subcomponents in your editor..
    }
    using Resource = juce::WebBrowserComponent::Resource;
    std::optional<Resource> AudioPluginAudioProcessorEditor::getResource(const juce::String &url)
    {
#ifdef RESOURCE_FILE_ROOT_PATH
        static const auto resourceFileRoot = juce::File(RESOURCE_FILE_ROOT_PATH);
#else
        // Fallback: assume the current working directory is the project root
        // and locate `plugin/ui/public` relative to it. This is useful when
        // running from an IDE or during development.
        static const auto resourceFileRoot = juce::File::getCurrentWorkingDirectory().getChildFile("plugin/ui/public");
        std::cout << resourceFileRoot.getFullPathName() << std::endl;
#endif
        const auto resourceToRetrieve = url == "/" ? "index.html" : url.fromFirstOccurrenceOf("/", false, false);
        const auto resource = resourceFileRoot.getChildFile(resourceToRetrieve).createInputStream();
        if (resource)
        {
            const auto extension = resourceToRetrieve.fromLastOccurrenceOf(".", false, false);
            return Resource{streamToVector(*resource), getMimeForExtension(extension)};
        }
        return std::nullopt;
    }
}