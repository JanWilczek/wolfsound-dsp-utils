#include <gtest/gtest.h>
#include <wolfsound/dsp/wolfsound_testSignals.hpp>
#include <wolfsound/file/wolfsound_WavFileWriter.hpp>

namespace wolfsound {
TEST(TestSignals, GenerateSineAndWriteToFile) {
  using namespace std::chrono_literals;
  constexpr auto SAMPLE_RATE = 44100_Hz;

  const auto sine = wolfsound::generateSine(440_Hz, SAMPLE_RATE, 5s);

  WavFileWriter::writeToFile("sine440Hz.wav", sine, SAMPLE_RATE);
}
}  // namespace wolfsound
