# WolfSound DSP Utils

A collection of header-only utilities for audio signal processing in C++.

> [!WARNING]
> This repository is a work-in-progress. 🛠️ Some features are not tested, others are subject to change. Use at your own risk!

## 🚀 Usage

This repository supports the CMake-based workflow. You can use it as a submodule or with a C++ package manager like CPM.

Once on your disk, you should `add_subdirectory()` on it and then link against `wolfsound::wolfsound_dsp_utils` target.

```cmake
target_link_libraries(
  MyAwesomePlugin
  PRIVATE wolfsound::wolfsound_dsp_utils
)
```

The library does not automatically manage JUCE so if you're receiving compiler or warning errors, it means that you're using features that need JUCE. You may need to link against some JUCE modules (see below).

## 🔗 Dependencies

`ProcessorFileIoTest`, `WavFileReader` and `WavFileWriter` depend on `juce::juce_core` and `juce::juce_audio_formats`. You need to link against them yourself. See _tests/CMakeLists.txt_ for usage example.

```cmake
target_link_libraries(
  MyAwesomePlugin
  PRIVATE wolfsound::wolfsound_dsp_utils
          juce::juce_core
          juce::juce_audio_formats
)

target_compile_definitions(MyAwesomePlugin PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
```

## 📋 License

This repository is MIT-licensed.

