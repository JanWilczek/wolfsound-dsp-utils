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
#include <wolfsound/common/wolfsound_assert.hpp>
#include <wolfsound/common/wolfsound_Interval.hpp>
#include <cmath>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <ratio>
#include <vector>

namespace wolfsound {
class MidiNoteNumber;

class Frequency {
public:
  constexpr Frequency() = default;

  explicit constexpr Frequency(float hz) : frequencyInHz_{hz} {
    WS_ASSERT(hz >= 0.f, "negative frequencies are not supported");
  }

  template <typename NumericType>
  constexpr Frequency& operator/=(NumericType rhs) {
    static_assert(std::is_arithmetic_v<NumericType>);
    WS_PRECONDITION(std::abs(rhs) > 0.f);

    frequencyInHz_ /= static_cast<decltype(frequencyInHz_)>(rhs);
    return *this;
  }

  template <typename NumericType>
  constexpr Frequency& operator*=(NumericType rhs) {
    static_assert(std::is_arithmetic<NumericType>::value);

    frequencyInHz_ *= static_cast<decltype(frequencyInHz_)>(rhs);
    return *this;
  }

  /** @brief Returns the frequency value in hertz.
   *
   *  This function is explicitly given NOT as a float conversion
   *   (operator float()) in order to avoid implicit conversion from an object
   * of this class to a float.
   */
  [[nodiscard]] constexpr float value() const noexcept {
    return frequencyInHz_;
  }

  /** @brief Returns the string representation of this frequency.
   *
   * The precision is limited to the given below decimal places.
   */
  [[nodiscard]] std::string toString() const {
    std::stringstream ss;

    // Set precision to 3 decimal places
    ss << std::fixed << std::setprecision(3) << frequencyInHz_;

    return ss.str();
  }

  template <class Ratio>
  [[nodiscard]] Frequency transposedBy(const Interval<Ratio>& rhs) const {
    return Frequency{frequencyInHz_ * rhs.frequencyRatio()};
  }

  [[nodiscard]] MidiNoteNumber midi() const;

private:
  float frequencyInHz_{0.f};
};

template <typename NumericType>
[[nodiscard]] constexpr Frequency operator/(Frequency lhs, NumericType rhs) {
  static_assert(std::is_arithmetic_v<NumericType>);

  lhs /= rhs;
  return lhs;
}

template <typename NumericType>
[[nodiscard]] constexpr Frequency operator*(Frequency lhs, NumericType rhs) {
  static_assert(std::is_arithmetic_v<NumericType>);

  lhs *= rhs;
  return lhs;
}

template <typename NumericType>
[[nodiscard]] constexpr Frequency operator*(NumericType lhs, Frequency rhs) {
  static_assert(std::is_arithmetic_v<NumericType>);

  return rhs * lhs;
}

[[nodiscard]] inline bool operator==(const Frequency& lhs,
                                     const Frequency& rhs) noexcept {
  constexpr auto DELTA = 1e-4f;
  return std::abs(lhs.value() - rhs.value()) < DELTA;
}

[[nodiscard]] constexpr bool operator>(const Frequency& lhs,
                                       const Frequency& rhs) noexcept {
  return lhs.value() > rhs.value();
}

[[nodiscard]] constexpr bool operator<=(const Frequency& lhs,
                                        const Frequency& rhs) noexcept {
  return lhs.value() <= rhs.value();
}

[[nodiscard]] constexpr bool operator>=(const Frequency& lhs,
                                        const Frequency& rhs) noexcept {
  return rhs <= lhs;
}

inline namespace literals {
constexpr Frequency operator""_Hz(long double frequencyInHz) {
  return Frequency{static_cast<float>(frequencyInHz)};
}

constexpr Frequency operator""_Hz(unsigned long long int frequencyInHz) {
  return Frequency{static_cast<float>(frequencyInHz)};
}

constexpr Frequency operator""_kHz(long double frequencyInKHz) {
  return Frequency{static_cast<float>(std::kilo::num * frequencyInKHz)};
}

constexpr Frequency operator""_kHz(unsigned long long int frequencyInKHz) {
  return Frequency{static_cast<float>(std::kilo::num * frequencyInKHz)};
}
}  // namespace literals

[[nodiscard]] inline const std::vector<Frequency>& octaveFrequencies() {
  using namespace literals;
  static const std::vector<Frequency> result{16_Hz,  31.5_Hz, 63_Hz, 125_Hz,
                                             250_Hz, 500_Hz,  1_kHz, 2_kHz,
                                             4_kHz,  8_kHz,   16_kHz};
  return result;
}

[[nodiscard]] inline const std::vector<std::string>& octaveFrequenciesLabels() {
  static const std::vector<std::string> labels{
      "16", "31.5", "63", "125", "250", "500", "1k", "2k", "4k", "8k", "16k"};
  return labels;
}

namespace constants {
inline const Frequency A4_FREQUENCY{440.f};
}

[[nodiscard]] inline Octaves octavesBetween(Frequency numerator,
                                            Frequency denominator) {
  return Octaves{std::log2(numerator.value() / denominator.value())};
}

/** Allows pretty printing of GoogleTest failure messages. */
inline void PrintTo(const Frequency& f, std::ostream* os) {  // NOLINT
  *os << f.toString() << " Hz";
}
}  // namespace wolfsound
