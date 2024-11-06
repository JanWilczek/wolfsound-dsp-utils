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
#include <algorithm>
#include <wolfsound/common/wolfsound_assert.hpp>

namespace wolfsound {
/** @brief A class representing a value in a closed range. Enforces that a given
 * value is in the given range [MinValue, MaxValue]. Ideal for clean API design.
 *
 *   Objects of this class are immutable.
 *
 *   The class provides value semantics so that it can be used in floating-point
 * computations without the need of calling extra conversion functions.
 *
 * TODO: Transition template arguments to float once Apple Clang supports it.
 */
template <int MinVal, int MaxVal, int DecimalPrecision = 0>
class ClosedRangeValue {
public:
  [[nodiscard]] static float min() noexcept {
    return withPrecisionApplied(MIN_VALUE);
  }

  [[nodiscard]] static float max() noexcept {
    return withPrecisionApplied(MAX_VALUE);
  }

  ClosedRangeValue(  // NOLINT: Allow implicit conversion from float.
      float value) noexcept
      : value_{
            std::clamp(value * precisionMultiplier(), MIN_VALUE, MAX_VALUE)} {
    WS_PRECONDITION(value * precisionMultiplier() >= MIN_VALUE);
    WS_PRECONDITION(value * precisionMultiplier() <= MAX_VALUE);
  }

  constexpr operator float()  // NOLINT: Allow implicit conversion to float.
      const noexcept {
    return withPrecisionApplied(value_);
  }

private:
  static constexpr auto MIN_VALUE = static_cast<float>(MinVal);
  static constexpr auto MAX_VALUE = static_cast<float>(MaxVal);

  [[nodiscard]] static float precisionMultiplier() noexcept {
    static const auto multiplier =
        std::pow(10.f, static_cast<float>(DecimalPrecision));
    return multiplier;
  }

  [[nodiscard]] static float withPrecisionApplied(float value) noexcept {
    return value / precisionMultiplier();
  }

  float value_;
};
}  // namespace wolfsound
