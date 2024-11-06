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
#include <cmath>
#include <iomanip>
#include <sstream>

namespace wolfsound {
class DecibelsFullScale {
public:
  constexpr DecibelsFullScale() = default;

  explicit constexpr DecibelsFullScale(float decibelsFullScale)
      : decibelsFullScale_{decibelsFullScale} {}

  /** @brief Returns the value in decibels full scale.
   *
   *  This function is explicitly given NOT as a float conversion
   *   (operator float()) in order to avoid implicit conversion from an object
   * of this class to a float.
   */
  [[nodiscard]] constexpr float value() const noexcept {
    return decibelsFullScale_;
  }

  [[nodiscard]] DecibelsFullScale operator-() const noexcept {
    return DecibelsFullScale{-decibelsFullScale_};
  }

  [[nodiscard]] std::string toString() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << decibelsFullScale_;
    return ss.str();
  }

  constexpr DecibelsFullScale& operator+=(
      const DecibelsFullScale& rhs) noexcept {
    decibelsFullScale_ += rhs.value();
    return *this;
  }

private:
  // TODO: Change representation to integer with 1 decimal place precision
  float decibelsFullScale_{0.f};
};

/** @brief Checks two decibel values for equality.
 *
 * Decibel values should be considered equal if they differ
 * only beyong the first decimal place.
 */
[[nodiscard]] inline bool operator==(const DecibelsFullScale& lhs,
                                     const DecibelsFullScale& rhs) {
  constexpr auto DECIMAL_PLACE_PRECISION = 1;
  constexpr auto PRECISION_MULTIPLIER = DECIMAL_PLACE_PRECISION * 10;
  const auto lhsRepresentation =
      static_cast<int>(std::round(PRECISION_MULTIPLIER * lhs.value()));
  const auto rhsRepresentation =
      static_cast<int>(std::round(PRECISION_MULTIPLIER * rhs.value()));
  return lhsRepresentation == rhsRepresentation;
}

[[nodiscard]] constexpr bool operator>(const DecibelsFullScale& lhs,
                                       const DecibelsFullScale& rhs) noexcept {
  return lhs.value() > rhs.value();
}

[[nodiscard]] constexpr bool operator<(const DecibelsFullScale& lhs,
                                       const DecibelsFullScale& rhs) noexcept {
  return rhs > lhs;
}

[[nodiscard]] constexpr bool operator<=(const DecibelsFullScale& lhs,
                                        const DecibelsFullScale& rhs) noexcept {
  return lhs.value() <= rhs.value();
}

[[nodiscard]] constexpr bool operator>=(const DecibelsFullScale& lhs,
                                        const DecibelsFullScale& rhs) noexcept {
  return rhs <= lhs;
}

[[nodiscard]] constexpr DecibelsFullScale operator+(
    DecibelsFullScale lhs,
    const DecibelsFullScale& rhs) noexcept {
  lhs += rhs;
  return lhs;
}

inline namespace literals {
constexpr DecibelsFullScale operator""_dBFS(long double literalValue) {
  return DecibelsFullScale{static_cast<float>(literalValue)};
}

constexpr DecibelsFullScale operator""_dBFS(
    unsigned long long int literalValue) {
  return DecibelsFullScale{static_cast<float>(literalValue)};
}
}  // namespace literals

/** @brief Allows pretty printing of GoogleTest failure messages. */
inline void PrintTo(const DecibelsFullScale& f, std::ostream* os) {  // NOLINT
  *os << std::setw(5) << f.value() << " dBFS";
}
}  // namespace wolfsound
