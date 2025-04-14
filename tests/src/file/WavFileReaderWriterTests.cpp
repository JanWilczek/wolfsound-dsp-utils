#include <gtest/gtest.h>
#include <wolfsound/file/wolfsound_WavFileWriter.hpp>
#include <wolfsound/file/wolfsound_WavFileReader.hpp>
#include "wolfsound/dsp/wolfsound_testSignals.hpp"

namespace wolfsound {
TEST(WavFileReaderWriter, WriteAndReadFilePreservesContent) {
  using namespace std::chrono_literals;

  // given
  constexpr auto SAMPLE_RATE = 48000_Hz;
  constexpr auto SEED = 0u;
  const auto testSignal = generateWhiteNoise(SAMPLE_RATE, 1s, SEED);

  const auto testFile =
      juce::File::getSpecialLocation(
          juce::File::SpecialLocationType::currentExecutableFile)
          .getParentDirectory()
          .getChildFile("noise.wav");
  const auto testFilePath = testFile.getFullPathName().toStdString();

  WavFileWriter::writeToFile(testFilePath, testSignal, SAMPLE_RATE);

  // when
  WavFileReader reader{};
  EXPECT_TRUE(reader.loadFile(testFile));

  // then
  EXPECT_EQ(reader.getLengthInSamples(), testSignal.size());
  for (const auto i : std::views::iota(0u, reader.getLengthInSamples())) {
    constexpr auto TOLERANCE = 1e-4f;
    EXPECT_NEAR(testSignal[i], reader.getSamples().getSample(0, int(i)),
                TOLERANCE);
  }

  // cleanup
  testFile.deleteFile();
}
}  // namespace wolfsound
