project(WolfSoundDspUtilsTests)
enable_testing()

cpmaddpackage(
  NAME
  GOOGLETEST
  GITHUB_REPOSITORY
  google/googletest
  VERSION
  1.16.0
  SOURCE_DIR
  ${LIB_DIR}/googletest
  OPTIONS
  "INSTALL_GTEST OFF"
  "gtest_force_shared_crt ON"
)

cpmaddpackage(
  NAME
  JUCE
  GIT_TAG
  8.0.12
  GITHUB_REPOSITORY
  juce-framework/JUCE
  SOURCE_DIR
  ${LIB_DIR}/juce
)

add_executable(
  WolfSoundDspUtilsTests
  src/common/MidiNoteNumberTests.cpp
  src/dsp/FractionalDelayLineTests.cpp
  src/dsp/TestSignalsTests.cpp
  src/file/WavFileReaderWriterTests.cpp
  src/juce/callOnMessageThreadIfNotNullTests.cpp
  src/juce/ParameterHolderTests.cpp
)

target_link_libraries(
  WolfSoundDspUtilsTests
  PRIVATE GTest::gtest_main
          wolfsound::wolfsound_dsp_utils
          juce::juce_core
          juce::juce_audio_formats
          juce::juce_events
          juce::juce_audio_processors
)

target_compile_definitions(WolfSoundDspUtilsTests PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)

include(GoogleTest)
gtest_discover_tests(WolfSoundDspUtilsTests)
