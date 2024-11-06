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
#include <ratio>
#include <cmath>

namespace wolfsound {
template <class R = std::ratio<1>>
class Interval {
public:
  using Ratio = R;

  explicit constexpr Interval(float ticks) : ticks_{ticks} {}
  [[nodiscard]] constexpr float count() const noexcept { return ticks_; }

  /** @return the ratio of frequencies corresponding to this interval,
   * for example, 2 for an octave, 4 for 2 octaves.
   */
  [[nodiscard]] constexpr float frequencyRatio() const;

  constexpr Interval& operator*=(float rhs) noexcept {
    ticks_ *= rhs;
    return *this;
  }

  [[nodiscard]] constexpr Interval<Ratio> operator-() const noexcept {
    return Interval<Ratio>{-ticks_};
  }

private:
  float ticks_;
};

template <class Ratio>
[[nodiscard]] constexpr Interval<Ratio> abs(const Interval<Ratio>& m) {
  return Interval<Ratio>{std::abs(m.count())};
}

template <class Ratio>
[[nodiscard]] constexpr Interval<Ratio> operator*(Interval<Ratio> lhs,
                                                  float rhs) {
  lhs *= rhs;
  return lhs;
}

template <class ToInterval, class Ratio>
[[nodiscard]] constexpr ToInterval intervalCast(
    const Interval<Ratio>& interval) {
  using ResultingRatio = std::ratio_divide<Ratio, typename ToInterval::Ratio>;
  return ToInterval{interval.count() * ResultingRatio::num /
                    ResultingRatio::den};
}

using Octaves = Interval<std::ratio<1>>;
using Semitones = Interval<std::ratio<1, 12>>;
using Cents = Interval<std::ratio<1, 1200>>;

namespace constants {
constexpr auto UNISON = Semitones{0.f};
constexpr auto OCTAVE = intervalCast<Semitones>(Octaves{1});
}  // namespace constants

template <class Ratio>
[[nodiscard]] constexpr float Interval<Ratio>::frequencyRatio() const {
  return std::pow(2.f, intervalCast<Octaves>(*this).count());
}

template <class Ratio>
[[nodiscard]] constexpr bool operator==(const Interval<Ratio>& lhs,
                                        const Interval<Ratio>& rhs) {
  return lhs.count() == rhs.count();
}

template <class Ratio>
[[nodiscard]] constexpr bool operator>=(const Interval<Ratio>& lhs,
                                        const Interval<Ratio>& rhs) {
  return lhs.count() >= rhs.count();
}

template <class Ratio>
[[nodiscard]] constexpr bool operator<=(const Interval<Ratio>& lhs,
                                        const Interval<Ratio>& rhs) {
  return lhs.count() <= rhs.count();
}

template <class Ratio>
[[nodiscard]] constexpr Interval<Ratio> operator*(float lhs,
                                                  Interval<Ratio> rhs) {
  rhs *= lhs;
  return rhs;
}
}  // namespace wolfsound
