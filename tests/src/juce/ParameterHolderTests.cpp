#include <wolfsound/juce/wolfsound_ParameterHolder.hpp>
#include <wolfsound/test/wolfsound_TestAudioProcessorBase.h>
#include <gtest/gtest.h>

namespace wolfsound {
namespace {
class ParameterHolderAudioProcessor : public TestAudioProcessorBase {
public:
  explicit ParameterHolderAudioProcessor(
      JuceParameterHolder::Builder builder = {})
      : floatParam{builder.add<juce::AudioParameterFloat>(
            "floatParam",
            "Float Param",
            juce::NormalisableRange{1.f, 10.f},
            5.f)},
        boolParam{builder.add<juce::AudioParameterBool>("boolParam",
                                                        "Bool Param",
                                                        true)},
        intParam{builder.add<juce::AudioParameterInt>("intParam",
                                                      "Int Param",
                                                      5,
                                                      10,
                                                      6)},
        choiceParam{builder.add<juce::AudioParameterChoice>(
            "choiceParam",
            "Choice Param",
            juce::StringArray{"choice 0", "choice 1", "choice 2"},
            1)},
        parameterHolder{std::move(builder).build(*this)} {}

  void getStateInformation(juce::MemoryBlock& block) override {
    const auto params = toVarArray(parameterHolder);
    juce::MemoryOutputStream output{block, true};
    juce::JSON::writeToStream(output, params);
  }

  void setStateInformation(const void* data, int size) override {
    juce::MemoryInputStream input{data, static_cast<size_t>(size), false};
    const auto parametersVar = juce::JSON::parse(input);
    ASSERT_TRUE(parametersVar.isArray());
    update(parameterHolder, *parametersVar.getArray());
  }

  juce::AudioParameterFloat& floatParam;
  juce::AudioParameterBool& boolParam;
  juce::AudioParameterInt& intParam;
  juce::AudioParameterChoice& choiceParam;
  JuceParameterHolder parameterHolder;
};
}  // namespace

TEST(ParameterHolder, CorrectlyAddsParameters) {
  const ParameterHolderAudioProcessor processor;
  ASSERT_EQ(4u, processor.getParameters().size());
}

TEST(ParameterHolder, CorrectlyRestoresState) {
  juce::MemoryBlock state;

  {
    ParameterHolderAudioProcessor processor;
    processor.floatParam = 2.f;
    processor.boolParam = false;
    processor.intParam = 7;
    processor.choiceParam = 2;
    processor.getStateInformation(state);
  }
  {
    ParameterHolderAudioProcessor processor;
    processor.setStateInformation(state.getData(),
                                  static_cast<int>(state.getSize()));

    EXPECT_FLOAT_EQ(2.f, processor.floatParam.get());
    EXPECT_FALSE(processor.boolParam.get());
    EXPECT_EQ(7, processor.intParam.get());
    EXPECT_EQ("choice 2",
              processor.choiceParam.getCurrentChoiceName().toStdString());
  }
}
}  // namespace wolfsound
