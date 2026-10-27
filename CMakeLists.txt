cmake_minimum_required(VERSION 3.22)
if(CMAKE_HOST_APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS Universal architectures")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")
endif()
project(PruedFX VERSION 2.0.4 LANGUAGES C CXX)
set(_juce_default "$ENV{JUCE_DIR}")
if(NOT _juce_default AND WIN32)
    set(_juce_default "D:/JUICED/JUCE")
elseif(NOT _juce_default)
    set(_juce_default "${CMAKE_CURRENT_SOURCE_DIR}/external/JUCE")
endif()
set(JUCE_DIR "${_juce_default}" CACHE PATH "JUCE 8.0.12 checkout")
if(NOT EXISTS "${JUCE_DIR}/modules/juce_core/juce_core.h")
    message(FATAL_ERROR "JUCE not found. Configure with -DJUCE_DIR=/path/to/JUCE-8.0.12")
endif()
add_subdirectory("${JUCE_DIR}" "${CMAKE_BINARY_DIR}/JUCE")
option(PRUED_BUILD_AU "Also build macOS Audio Unit for Logic Pro" OFF)
set(_prued_formats VST3)
if(PRUED_BUILD_AU)
    if(NOT APPLE)
        message(FATAL_ERROR "PRUED_BUILD_AU is only supported on macOS")
    endif()
    list(APPEND _prued_formats AU)
endif()
juce_add_binary_data(PruedArtwork SOURCES
    assets/tip-kofi.png assets/background.png assets/background2.png assets/knob1.png
    assets/knob2.png assets/knob3.png assets/knob4.png assets/JS-Thanaporn-Normal.ttf)
juce_add_plugin(PruedFX
    COMPANY_NAME "Yuay Sound Records"
    COMPANY_WEBSITE "www.YuaySoundRecords.com"
    BUNDLE_ID "com.YuaySoundRecords.PruedFX"
    PLUGIN_MANUFACTURER_CODE Manu PLUGIN_CODE YUAY
    PRODUCT_NAME "Prued FX" DESCRIPTION "One Knob Stutter"
    FORMATS ${_prued_formats} AU_MAIN_TYPE kAudioUnitType_MusicEffect
    IS_SYNTH FALSE NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE COPY_PLUGIN_AFTER_BUILD FALSE)
juce_generate_juce_header(PruedFX)
target_sources(PruedFX PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp
    Source/StutterEngine.cpp Source/PerformanceLooper.cpp Source/filmstripknob.cpp)
target_compile_features(PruedFX PUBLIC cxx_std_17)
target_compile_definitions(PruedFX PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(PruedFX PRIVATE PruedArtwork juce::juce_audio_utils
    PUBLIC juce::juce_recommended_config_flags)

include(CTest)
if(BUILD_TESTING)
    juce_add_console_app(PerformanceTests PRODUCT_NAME "Prued FX Tests")
    juce_generate_juce_header(PerformanceTests)
    target_sources(PerformanceTests PRIVATE tests/PerformanceTests.cpp tests/reference/RecoveredPluginEditor.cpp)
    target_include_directories(PerformanceTests PRIVATE Source)
    target_compile_features(PerformanceTests PRIVATE cxx_std_17)
    target_link_libraries(PerformanceTests PRIVATE PruedFX PruedArtwork juce::juce_audio_utils)
    add_test(NAME PerformanceTests COMMAND PerformanceTests)
    add_test(NAME FrozenWetMixRegression COMMAND PerformanceTests --wet-mix-only)
    add_test(NAME TransportAutomationStress COMMAND PerformanceTests --stress-only)
    add_test(NAME BypassAndKnobRegression COMMAND PerformanceTests --controls-only)
    juce_add_console_app(LicensingTests PRODUCT_NAME "Prued FX Licensing Tests")
    juce_generate_juce_header(LicensingTests)
    target_sources(LicensingTests PRIVATE tests/LicensingTests.cpp)
    target_include_directories(LicensingTests PRIVATE Source)
    target_compile_features(LicensingTests PRIVATE cxx_std_17)
    target_link_libraries(LicensingTests PRIVATE PruedFX PruedArtwork juce::juce_audio_utils)
    add_test(NAME LicensingTests COMMAND LicensingTests)
    add_test(NAME OriginalUIRegression COMMAND PerformanceTests --ui-only "${CMAKE_CURRENT_SOURCE_DIR}/reports/ui-preview.png")

    juce_add_console_app(VST3Smoke PRODUCT_NAME "Prued FX VST3 Smoke")
    target_sources(VST3Smoke PRIVATE tests/VST3Smoke.cpp)
    target_compile_features(VST3Smoke PRIVATE cxx_std_17)
    target_compile_definitions(VST3Smoke PRIVATE JUCE_PLUGINHOST_VST3=1 JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
    target_link_libraries(VST3Smoke PRIVATE juce::juce_audio_processors)
    add_test(NAME VST3Smoke COMMAND VST3Smoke "${CMAKE_CURRENT_BINARY_DIR}/PruedFX_artefacts/$<CONFIG>/VST3/Prued FX.vst3")
    if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/../recovered-1.0.0/Prued FX.vst3")
        add_test(NAME RecoveredVST3Compatibility COMMAND VST3Smoke
            "${CMAKE_CURRENT_BINARY_DIR}/PruedFX_artefacts/$<CONFIG>/VST3/Prued FX.vst3"
            "${CMAKE_CURRENT_BINARY_DIR}/../recovered-1.0.0/Prued FX.vst3")
    endif()
endif()
