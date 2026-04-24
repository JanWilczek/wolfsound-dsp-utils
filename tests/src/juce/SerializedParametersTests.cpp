#include <gtest/gtest.h>
#include <wolfsound/juce/wolfsound_SerializedParameters.hpp>

namespace wolfsound {
TEST(SerializedParameters, FromVarChecked) {
  const auto result = SerializedParameters::from(juce::JSON::parse(R"-(
{
  "name": "Min (Factory Preset)",
  "parameters": [
  {
    "id": "floatParam",
        "value": 1.0
  },
  {
    "id": "boolParam",
        "value": false
  },
  {
    "id": "intParam",
        "value": 5
  },
  {
    "id": "choiceParam",
        "value": "choice 0"
  }
  ]
}
)-"));

  EXPECT_TRUE(result.has_value());
}

TEST(SerializedParameters, FromVarArrayChecked) {
  const auto array = juce::JSON::parse(R"-(
[
  {
    "id": "floatParam",
        "value": 1.0
  },
  {
    "id": "boolParam",
        "value": false
  },
  {
    "id": "intParam",
        "value": 5
  },
  {
    "id": "choiceParam",
        "value": "choice 0"
  }
]
)-");
  ASSERT_TRUE(array.isArray());

  const auto result = SerializedParameters::from(*array.getArray());

  EXPECT_TRUE(result.has_value());
}
}  // namespace wolfsound