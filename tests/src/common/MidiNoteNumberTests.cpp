#include <gtest/gtest.h>
#include <wolfsound/common/wolfsound_MidiNoteNumber.hpp>

namespace wolfsound {
TEST(MidiNoteNumber, Identities) {
  ASSERT_EQ(440_Hz, MidiNoteNumber{69.f}.hz());
  ASSERT_EQ(880_Hz, MidiNoteNumber{81.f}.hz());
}
}  // namespace wolfsound
