#pragma once

#include <wolfsound/common/wolfsound_Frequency.hpp>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>

namespace wolfsound {
class WavFileReader {
public:
  using Frequency = wolfsound::Frequency;

  bool loadFile(const juce::File& file);
  [[nodiscard]] int getNumChannels() const;
  [[nodiscard]] Frequency getSampleRate() const;
  [[nodiscard]] std::size_t getLengthInSamples() const;

  [[nodiscard]] const juce::AudioBuffer<float>& getSamples() const;

private:
  juce::AudioBuffer<float> audioBuffer_;
  double sampleRate_ = 0.0;
};

inline bool WavFileReader::loadFile(const juce::File& file) {
  juce::AudioFormatManager formatManager;
  formatManager.registerBasicFormats();  // Registers WAV, AIFF, etc.

  std::unique_ptr<juce::AudioFormatReader> reader(
      formatManager.createReaderFor(file));

  if (reader == nullptr) {
    throw std::runtime_error{"Could not open file: " +
                             file.getFullPathName().toStdString()};
    return false;
  }

  audioBuffer_.setSize(static_cast<int>(reader->numChannels),
                       static_cast<int>(reader->lengthInSamples));
  reader->read(&audioBuffer_, 0, audioBuffer_.getNumSamples(), 0, true, true);

  sampleRate_ = reader->sampleRate;

  DBG("File loaded successfully: " + file.getFullPathName());
  return true;
}

inline int WavFileReader::getNumChannels() const {
  return audioBuffer_.getNumChannels();
}

inline auto WavFileReader::getSampleRate() const -> Frequency {
  return Frequency{static_cast<float>(sampleRate_)};
}

inline std::size_t WavFileReader::getLengthInSamples() const {
  return static_cast<std::size_t>(audioBuffer_.getNumSamples());
}

inline const juce::AudioBuffer<float>& WavFileReader::getSamples() const {
  return audioBuffer_;
}
}  // namespace wolfsound
