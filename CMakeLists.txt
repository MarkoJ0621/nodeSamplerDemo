cmake_minimum_required(VERSION 3.22 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20)

project(NodeSampler VERSION 1.0.0)

add_subdirectory("./juce" "${CMAKE_BINARY_DIR}/juce")

# 1. Create the plugin target first
juce_add_plugin(NodeSampler 
    COMPANY_NAME "akitoFans"
    IS_SYNTH TRUE 
    NEEDS_MIDI_INPUT TRUE 
    NEEDS_MIDI_OUTPUT FALSE 
    PLUGIN_MANUFACTURER_CODE Akns
    PLUGIN_CODE Akns
    FORMATS VST3 AU Standalone
)

# 2. Then attach your sources
target_sources(NodeSampler
    PRIVATE
        plugin/src/PluginEditor.cpp
        plugin/src/PluginProcessor.cpp
)

# 3. Then link JUCE modules
target_link_libraries(NodeSampler
    PRIVATE 
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_dsp
)

# 4. Extra defines
target_compile_definitions(NodeSampler
    PUBLIC 
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# 5. Generate JUCE headers last
juce_generate_juce_header(NodeSampler)
