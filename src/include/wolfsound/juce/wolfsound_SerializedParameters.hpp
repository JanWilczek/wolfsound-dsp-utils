/**

                                     +++++
                                 +++
                              =++      ++
                             ++     +=      +++                ++
                            ++    ++        ++ +++             ++
                            +    ++   ++   +++   ++++++++    +++
                           ++   ++   ++     ++++         +++++++
                           +    +    +      *+++++           +++
                           +            ++++    +++         +++
                                        +++++    ++        ++
                                        +++  ++++*         ++
                                          ++++++          ++
                                               +++         +
                                                +++        ++
                                                 +++        +++
+++= =+++  +++=         +++   ++++=======         ++          ++           ====
++++ ++++ ++++          +++  ++++ ========                      ++         ====
++++ ++++ ++++ ++++++   +++ +++++++++=      +++++=  ++++ +++ +++=+++=  =++==+++
 ++++++++++++ ++++++++  +++ +++++ =+++++   +++=++++ ++++ +++ ++++=++++ ++++++++
 ++++++++++++ +++  +++  +++  +++    ++++++++++ ++++ ++++ +++ ++++ ++++ ++++++++
 ***+*+++++++ **+  +*+  ***  ***      ++++++++ =+++ ++++ +++ ++++ ++++ ++++++++
  ***** ****+ *** ****  ***  *** ++++ ++++ +++ ++++ ++++ +++ ++++ ++++ ++++++++
  ****  ****   ******   ***  ***  ++++++++ +++++++   +++++++ ++++ ++++ ++++++++
                                     *
             ____                         _   _   _     _   _
            / ___|    _       _          | | | | | |_  (_) | |  ___
           | |      _| |_   _| |_        | | | | | __| | | | | / __|
           | |___  |_   _| |_   _|       | |_| | | |_  | | | | \__ \
            \____|   |_|     |_|          \___/   \__| |_| |_| |___/


  WolfSound C++ Utils

  License:

  MIT License

  Copyright (c) 2024 Jan Wilczek

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/
#pragma once
#include <optional>
#include <ranges>
#include <juce_core/juce_core.h>

namespace wolfsound {
/** @brief Wrapper around a properly structured juce::var containing parameter
   IDs and their values. Use it for storing parameter values.

Conceptually, this class holds a JSON object structured as follows
    @code
{
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
@endcode
        */
class SerializedParameters {
public:
  /** @brief Return an object of this class if the input is a
     juce::DynamicObject with a "parameters" field (see class description),
     std::nullopt otherwise */
  [[nodiscard]] static std::optional<SerializedParameters> from(
      const juce::var&);

  /** @brief Return an object of this class if the input is an array filled with
     juce::DynamicObject objects, each having an "id" field and a "value" field,
     std::nullopt otherwise */
  [[nodiscard]] static std::optional<SerializedParameters> from(
      const juce::Array<juce::var>&);

  /** @brief Return a juce::DynamicObject with the structure as shown in the
   * class description */
  [[nodiscard]] const juce::var& toVar() const noexcept { return _impl; }

  /** @brief Return the value of the "parameters" field with the structure as
   * shown in the class description */
  [[nodiscard]] const juce::Array<juce::var>& toVarArray() const noexcept {
    return *_impl.getProperty("parameters", juce::var{juce::Array<juce::var>{}})
                .getArray();
  }

private:
  [[nodiscard]] static SerializedParameters fromCorrect(
      const juce::Array<juce::var>&);

  explicit SerializedParameters(juce::var v) : _impl{std::move(v)} {}

  juce::var _impl;
};

inline SerializedParameters SerializedParameters::fromCorrect(
    const juce::Array<juce::var>& parameterArray) {
  const juce::DynamicObject::Ptr root{new juce::DynamicObject};
  root->setProperty("parameters", parameterArray);
  return SerializedParameters{{root}};
}

inline std::optional<SerializedParameters> SerializedParameters::from(
    const juce::var& v) {
  if (v.hasProperty("parameters") && v["parameters"].isArray()) {
    return SerializedParameters::from(*v["parameters"].getArray());
  }

  return {};
}

inline std::optional<SerializedParameters> SerializedParameters::from(
    const juce::Array<juce::var>& parameterArray) {
  auto hasIdAndValue = [](const auto& var) {
    return var.hasProperty("id") && var["id"].isString() &&
           var.hasProperty("value");
  };
  if (std::ranges::all_of(parameterArray, hasIdAndValue)) {
    return SerializedParameters::fromCorrect(parameterArray);
  }

  return {};
}
}  // namespace wolfsound