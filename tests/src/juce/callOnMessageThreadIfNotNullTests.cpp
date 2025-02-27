#include <gtest/gtest.h>
#include <wolfsound/juce/wolfsound_callOnMessageThreadIfNotNull.hpp>
#include <juce_events/juce_events.h>
#include <juce_core/juce_core.h>

namespace wolfsound {
TEST(callOnMessageThreadIfNotNull, ExecutesInlineWhenNoMessageThread) {
  bool called = false;

  callOnMessageThreadIfNotNull([&] { called = true; });

  ASSERT_TRUE(called);
}
}  // namespace wolfsound
