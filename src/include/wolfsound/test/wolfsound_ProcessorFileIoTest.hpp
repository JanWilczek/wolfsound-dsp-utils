#pragma once

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <cstddef>
#include <functional>
#include <wolfsound/file/wolfsound_WavFileReader.hpp>
#include <wolfsound/file/wolfsound_WavFileWriter.hpp>
#include <wolfsound/common/wolfsound_Frequency.hpp>
#include <wolfsound/common/wolfsound_assert.hpp>

namespace wolfsound {
namespace detail {
template <typename SampleType>
std::vector<SampleType> vectorFromChannel(
    const juce::dsp::AudioBlock<SampleType>& audioBlock,
    std::size_t channel = 0u) {
  WS_PRECONDITION(channel < audioBlock.getNumChannels());
  std::vector<SampleType> samples(
      audioBlock.getChannelPointer(channel),
      audioBlock.getChannelPointer(channel) + audioBlock.getNumSamples());
  return samples;
}
}  // namespace detail

template <class Processor>
class ProcessorFileIoTest {
public:
  static constexpr auto CHANNEL_COUNT = 1u;

  using SampleType = float;

  struct Spec {
    /** @brief If relative, audioInputFilesDirectoryPath is taken as the parent.
     */
    std::string inputAudioFile;

    std::string name = "";

    std::function<void(Processor&)> preProcessCallback = [](auto&) {};

    std::string audioInputFilesDirectoryPath = "";

    /** @brief If it is empty, then the input file directory is used. */
    std::string audioOutputFilesDirectoryPath = "";
  };

  explicit ProcessorFileIoTest(Spec spec) : spec_{std::move(spec)} {}

  void run() {
    using namespace juce::dsp;

    // read the input file
    const auto inputFile = getInputFile();
    wavReader_.loadFile(inputFile);

    // create and prepare the processor
    Processor processor;
    processor.prepare(ProcessSpec{
        .sampleRate = static_cast<double>(getSampleRate().value()),
        .maximumBlockSize =
            static_cast<juce::uint32>(wavReader_.getLengthInSamples()),
        .numChannels = CHANNEL_COUNT,
    });

    spec_.preProcessCallback(processor);

    // render the output
    auto audioBlock = getAudioBlock();
    processor.process(ProcessContextReplacing<SampleType>{audioBlock});

    // write the output to file
    const auto samples = detail::vectorFromChannel(audioBlock);
    WavFileWriter::writeToFile(getOutputFilePath(), samples, getSampleRate());
  }

  [[nodiscard]] wolfsound::Frequency getSampleRate() const {
    return wavReader_.getSampleRate();
  }

  [[nodiscard]] std::size_t getOutputSamplesCount() const {
    return wavReader_.getLengthInSamples();
  }

private:
  static constexpr auto FIRST_CHANNEL = 0u;

  [[nodiscard]] juce::File getInputFile() const {
    if (!juce::File::isAbsolutePath(spec_.inputAudioFile)) {
      juce::File inputDirectory{spec_.audioInputFilesDirectoryPath};
      return inputDirectory.getChildFile(spec_.inputAudioFile);
    }
    return {spec_.inputAudioFile};
  }

  [[nodiscard]] std::string getOutputFilename() const {
    auto inputFilename = spec_.inputAudioFile;

    static const std::string extension{".wav"};
    if (inputFilename.ends_with(extension)) {
      static const auto extensionLength =
          static_cast<decltype(extension)::difference_type>(extension.length());
      inputFilename.replace(inputFilename.end() - extensionLength,
                            inputFilename.end(), "");
    }

    return inputFilename + "_" + spec_.name + "Output.wav";
  }

  [[nodiscard]] juce::File getInputDirectory() const {
    return getInputFile().getParentDirectory();
  }

  [[nodiscard]] juce::File getOutputDirectory() const {
    if (juce::File::isAbsolutePath(spec_.audioOutputFilesDirectoryPath)) {
      return {spec_.audioOutputFilesDirectoryPath};
    }

    return getInputDirectory();
  }

  [[nodiscard]] std::string getOutputFilePath() const {
    const auto outputFilename = getOutputFilename();

    if (juce::File::isAbsolutePath(outputFilename)) {
      return getOutputDirectory()
          .getChildFile(juce::File{outputFilename}.getFileName())
          .getFullPathName()
          .toStdString();
    }

    return getOutputDirectory()
        .getChildFile(getOutputFilename())
        .getFullPathName()
        .toStdString();
  }

  [[nodiscard]] juce::dsp::AudioBlock<SampleType> getAudioBlock() {
    samplesToProcess_ = wavReader_.getSamples();
    // take only the first channel
    return juce::dsp::AudioBlock<SampleType>{samplesToProcess_}
        .getSubsetChannelBlock(FIRST_CHANNEL, CHANNEL_COUNT);
  }

  Spec spec_;
  WavFileReader wavReader_;
  juce::AudioBuffer<SampleType> samplesToProcess_;
};
}  // namespace wolfsound
