cmake_minimum_required (VERSION 3.15)

set (CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum OS X deployment version")
set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (OomphSC VERSION "0.1.0")

set (CMAKE_CXX_STANDARD 17)

add_subdirectory (JUCE)

juce_add_plugin (OomphSC
    PLUGIN_MANUFACTURER_CODE daru
    PLUGIN_CODE OoSC
    COMPANY_NAME "Daniel Rudrich"
    PRODUCT_NAME "OomphSC"
    FORMATS VST3
    COPY_PLUGIN_AFTER_BUILD True)

juce_generate_juce_header (OomphSC)


target_sources (OomphSC PRIVATE
    src/Settings.hpp
    src/OSCComponent.hpp
    src/OSCSenderPlus.hpp
    src/PluginProcessor.hpp
    src/PluginProcessor.cpp
    src/PluginEditor.hpp
    src/PluginEditor.cpp
    src/Visualizer.hpp)

target_compile_definitions (OomphSC PUBLIC
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
    JUCE_DISPLAY_SPLASH_SCREEN=0
    DONT_SET_USING_JUCE_NAMESPACE=1
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries (OomphSC
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_osc
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
