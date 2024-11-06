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

  struct Args {
    std::string absolutePath;
    Frequency sampleRate;
  };

  explicit WavFileWriter(Args);

  void write(const std::vector<float>& samples) const;

private:
  [[nodiscard]] juce::File outputDirectory() const;

  juce::File file_;
  Frequency sampleRate_;
};

namespace detail {
class PtrArrayVectorWrapper {
public:
  explicit PtrArrayVectorWrapper(const std::vector<float>& singleChannelSamples)
      : singleChannelSamples_{singleChannelSamples},
        ptr_{singleChannelSamples_.data()} {}

  [[nodiscard]] const float* const* ptrArray() const noexcept { return &ptr_; }

private:
  const std::vector<float>& singleChannelSamples_;  // NOLINT
  const float* ptr_;
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
  WavFileWriter wavWriter{
      {.absolutePath = absolutePath, .sampleRate = sampleRate}};
  wavWriter.write(samples);
}

inline WavFileWriter::WavFileWriter(Args args)
    : file_{args.absolutePath}, sampleRate_{args.sampleRate} {}

inline void WavFileWriter::write(const std::vector<float>& samples) const {
  const auto filename =
      detail::sanitizeFilename(file_.getFileName().toStdString());
  auto outputFile = outputDirectory().getChildFile(filename);

  // if the file is in a directory that must be created, do it first
  outputFile.create();

  auto outStream = std::make_unique<juce::FileOutputStream>(outputFile);

  if (outStream->failedToOpen()) {
    throw std::runtime_error{"failed to open the file for writing"};
  }
  outStream->setPosition(0);
  outStream->truncate();

  juce::WavAudioFormat wavFormat;
  const auto writer =
      std::unique_ptr<juce::AudioFormatWriter>{wavFormat.createWriterFor(
          outStream.get(), static_cast<double>(sampleRate_.value()), 1u, 16, {},
          0)};
  if (writer == nullptr) {
    throw std::runtime_error{"failed to initialize WAV file writer"};
  }
  outStream.release();  // NOLINT: if we got here, JUCE will delete the stream

  detail::PtrArrayVectorWrapper wrapper{samples};
  writer->writeFromFloatArrays(wrapper.ptrArray(), 1,
                               static_cast<int>(samples.size()));
}
}  // namespace wolfsound
