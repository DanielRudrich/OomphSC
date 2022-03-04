cmake_minimum_required (VERSION 3.15)

set (CMAKE_OSX_DEPLOYMENT_TARGET "10.14" CACHE STRING "Minimum OS X deployment version")
set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set (CMAKE_POSITION_INDEPENDENT_CODE ON)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

project (OomphSC VERSION "0.3.0")

set (CMAKE_CXX_STANDARD 17)

add_subdirectory (JUCE)

juce_add_plugin (OomphSC
    PLUGIN_MANUFACTURER_CODE daru
    PLUGIN_CODE OoSC
    COMPANY_NAME "Daniel Rudrich"
    PRODUCT_NAME "OomphSC"
    FORMATS VST3
    COPY_PLUGIN_AFTER_BUILD True)

juce_add_binary_data (fonts
    HEADER_NAME "BinaryFonts.hpp"
    NAMESPACE "BinaryFonts"
    SOURCES "fonts/Montserrat-Regular.ttf")

juce_add_binary_data (logo
    HEADER_NAME "BinaryLogo.hpp"
    NAMESPACE "BinaryLogo"
    SOURCES "logo/logo.svg")

juce_generate_juce_header (OomphSC)


target_sources (OomphSC PRIVATE
    source/Settings.hpp
    source/OSCComponent.hpp
    source/OSCSenderPlus.hpp
    source/PluginProcessor.hpp
    source/PluginProcessor.cpp
    source/PluginEditor.hpp
    source/PluginEditor.cpp
    source/Visualizer.hpp
    source/PeakRMSButton.hpp
    source/Slider.hpp
    source/Fonts.hpp)

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
        fonts
        logo
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
