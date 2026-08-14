juce_add_plugin(NodeSampler 
    COMPANY_NAME  akitoFans
    IS_SYNTH TRUE 
    NEEDS_MIDI_INPUT TRUE 
    NEEDS_MIDI_OUTPUT FALSE 
    PLUGIN_MANUFACTURER_CODE Akns
    PLUGIN_CODE Akns
    FORMATS Standalone VST3 AU
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(NodeSampler
    PRIVATE src/PluginEditor.cpp src/PluginProcessor.cpp)

# Provide a compile-time macro with the plugin UI public path so the
# runtime can use a project-relative path when locating web UI resources.
target_compile_definitions(NodeSampler
    PRIVATE RESOURCE_FILE_ROOT_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/ui/public\"
)

target_include_directories(NodeSampler
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(NodeSampler
    PRIVATE 
        juce::juce_audio_utils
        juce::juce_gui_extra
)

target_compile_definitions(NodeSampler
    PUBLIC 
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

add_compile_definitions(JUCE_DEBUG=1)