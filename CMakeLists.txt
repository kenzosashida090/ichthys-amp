cmake_minimum_required(VERSION 3.22)

project(IchthysAmp VERSION 0.1.0)

add_subdirectory(external/JUCE)

juce_add_plugin(IchthysAmp
    COMPANY_NAME "Ichthys Audio"
    BUNDLE_ID "audio.ichthys.amp"
    PLUGIN_MANUFACTURER_CODE AstA
    PLUGIN_CODE As01
    PRODUCT_NAME "Ichthys Amp"

    FORMATS VST3 LV2
    LV2URI "https://ichthys.audio/plugins/amp"
    VST3_CAN_REPLACE_VST2 FALSE

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE

    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(IchthysAmp)

target_sources(IchthysAmp PRIVATE
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
)

target_compile_features(IchthysAmp PRIVATE cxx_std_17)

target_compile_definitions(IchthysAmp PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(IchthysAmp PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)
