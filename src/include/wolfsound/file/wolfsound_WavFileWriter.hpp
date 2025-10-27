#pragma once

#include <wolfsound/common/wolfsound_Frequency.hpp>
#include <wolfsound/common/wolfsound_assert.hpp>
#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <vector>
#include <stdexcept>

namespace wolfsound {
class WavFileWriter {
public:
  using Frequency = wolfsound::Frequency;

  /** @brief A shorthand for creating an instance and writing samples to it */
  static void writeToFile(const std::string& absolutePath,
                          const std::vector<float>& samples,
                          Frequency sampleRate);

  /** @brief A shorthand for creating an instance and writing samples to it */
  template <typename SampleType>
  static void writeToFile(const std::string& absolutePath,
                          juce::Span<const SampleType> samples,
                          Frequency sampleRate);

  struct Args {
    std::string absolutePath;
    Frequency sampleRate;
  };

  explicit WavFileWriter(Args);

  void write(const std::vector<float>& samples) const;

  template <typename SampleType>
  void write(juce::Span<const SampleType> samples) const;

private:
  [[nodiscard]] juce::File outputDirectory() const;

  juce::File file_;
  Frequency sampleRate_;
};

namespace detail {
template <typename SampleType>
class PtrArrayVectorWrapper {
public:
  explicit PtrArrayVectorWrapper(
      juce::Span<const SampleType> singleChannelSamples)
      : singleChannelSamples_{singleChannelSamples},
        ptr_{singleChannelSamples_.data()} {}

  [[nodiscard]] const float* const* ptrArray() const noexcept { return &ptr_; }

private:
  juce::Span<const SampleType> singleChannelSamples_;
  const SampleType* ptr_;
};

inline std::string sanitizeFilename(std::string filename) {
  if (!filename.ends_with(".wav")) {
    filename += ".wav";
  }

  return filename;
}
}  // namespace detail

inline juce::File WavFileWriter::outputDirectory() const {
  juce::File outputDirectory{file_.getParentDirectory()};
  const auto directoryCreationResult = outputDirectory.createDirectory();
  WS_ASSERT(directoryCreationResult.ok(),
            directoryCreationResult.getErrorMessage().toStdString().c_str());
  return outputDirectory;
}

inline void WavFileWriter::writeToFile(const std::string& absolutePath,
                                       const std::vector<float>& samples,
                                       Frequency sampleRate) {
  writeToFile(absolutePath, juce::Span{samples}, sampleRate);
}

template <typename SampleType>
inline void WavFileWriter::writeToFile(const std::string& absolutePath,
                                       juce::Span<const SampleType> samples,
                                       Frequency sampleRate) {
  WavFileWriter wavWriter{
      {.absolutePath = absolutePath, .sampleRate = sampleRate}};
  wavWriter.write(samples);
}

inline WavFileWriter::WavFileWriter(Args args)
    : file_{args.absolutePath}, sampleRate_{args.sampleRate} {}

inline void WavFileWriter::write(const std::vector<float>& samples) const {
  write(juce::Span{samples});
}

template <typename SampleType>
void WavFileWriter::write(juce::Span<const SampleType> samples) const {
  const auto filename =
      detail::sanitizeFilename(file_.getFileName().toStdString());
  auto outputFile = outputDirectory().getChildFile(filename);

  // if the file is in a directory that must be created, do it first
  outputFile.create();

  auto openAndTruncateFile =
      [](const juce::File& file) -> std::unique_ptr<juce::OutputStream> {
    auto outStream = std::make_unique<juce::FileOutputStream>(file);

    if (outStream->failedToOpen()) {
      throw std::runtime_error{"failed to open the file for writing"};
    }
    outStream->setPosition(0);
    outStream->truncate();
    return outStream;
  };

  auto outStream = openAndTruncateFile(outputFile);
  juce::WavAudioFormat wavFormat;
  const auto writer = wavFormat.createWriterFor(
      outStream, juce::AudioFormatWriterOptions{}
                     .withSampleRate(static_cast<double>(sampleRate_.value()))
                     .withNumChannels(1)
                     .withBitsPerSample(16)
                     .withQualityOptionIndex(0));
  if (writer == nullptr) {
    throw std::runtime_error{"failed to initialize WAV file writer"};
  }
  outStream.release();  // NOLINT: if we got here, JUCE will delete the stream

  detail::PtrArrayVectorWrapper wrapper{samples};
  writer->writeFromFloatArrays(wrapper.ptrArray(), 1,
                               static_cast<int>(samples.size()));
}
}  // namespace wolfsound
