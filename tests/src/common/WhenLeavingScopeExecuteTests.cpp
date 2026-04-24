#include <gtest/gtest.h>
#include <wolfsound/common/wolfsound_WhenLeavingScopeExecute.hpp>

namespace wolfsound {
TEST(WhenLeavingScopeExecute, CallsNonEmptyCallbackOnDestruction) {
  auto i = 0;
  {
    const WhenLeavingScopeExecute testee{[&]() { i = 1; }};
  }
  EXPECT_EQ(1, i);
}

TEST(WhenLeavingScopeExecute, DoesntCallEmptyCallbackOnDestruction) {
  EXPECT_NO_FATAL_FAILURE({ const WhenLeavingScopeExecute testee{nullptr}; });
}
}  // namespace wolfsound