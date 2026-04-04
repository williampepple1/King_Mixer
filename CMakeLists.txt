cmake_minimum_required(VERSION 3.22)
project(AssistedMixing VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        7.0.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(AssistedMixing
    COMPANY_NAME "KingMixer"
    PLUGIN_MANUFACTURER_CODE Kmix
    PLUGIN_CODE Kmxr
    FORMATS VST3 Standalone
    PRODUCT_NAME "King Mixer"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    ICON_BIG Resources/icon.png
    ICON_SMALL Resources/icon.png
)

target_sources(AssistedMixing
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/GainStage.cpp
        Source/DSP/ParametricEQ.cpp
        Source/DSP/Compressor.cpp
        Source/DSP/Saturation.cpp
        Source/DSP/StereoWidth.cpp
        Source/DSP/ReverbSend.cpp
        Source/Rules/MixRuleDatabase.cpp
        Source/Analysis/SpectrumAnalyzer.cpp
        Source/Analysis/LevelMeter.cpp
        Source/Analysis/WaveformBuffer.cpp
        Source/UI/CustomLookAndFeel.cpp
        Source/UI/EQPanel.cpp
        Source/UI/CompressorPanel.cpp
        Source/UI/SaturationPanel.cpp
        Source/UI/ReverbPanel.cpp
        Source/UI/GainMixPanel.cpp
)

target_include_directories(AssistedMixing
    PRIVATE
        Source
)

target_compile_definitions(AssistedMixing
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(AssistedMixing
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
