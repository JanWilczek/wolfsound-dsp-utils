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
#include <wolfsound/common/wolfsound_Frequency.hpp>
#include <wolfsound/common/wolfsound_ClosedRangeValue.hpp>
#include <wolfsound/common/wolfsound_Interval.hpp>

namespace wolfsound {
class MidiNoteNumber {
public:
  explicit MidiNoteNumber(float noteNumber) : noteNumber_{noteNumber} {}

  [[nodiscard]] Frequency hz() const;
  MidiNoteNumber& operator-=(const Semitones& rhs) {
    noteNumber_ = decltype(noteNumber_){noteNumber_ - rhs.count()};
    return *this;
  }

  [[nodiscard]] float value() const noexcept { return noteNumber_; }

  template <class IntervalType>
  MidiNoteNumber& operator+=(const IntervalType& interval) {
    noteNumber_ = decltype(noteNumber_){
        noteNumber_ + intervalCast<Semitones>(interval).count()};
    return *this;
  }

  friend Semitones operator-(MidiNoteNumber lhs, const MidiNoteNumber& rhs);

private:
  ClosedRangeValue<0, 127> noteNumber_;
};

namespace constants {
inline const MidiNoteNumber A4{69.f};
}

[[nodiscard]] inline bool operator==(const MidiNoteNumber& lhs,
                                     const MidiNoteNumber& rhs) {
  return std::abs(lhs.value() - rhs.value()) < 0.0001f;
}

[[nodiscard]] inline Semitones operator-(MidiNoteNumber lhs,
                                         const MidiNoteNumber& rhs) {
  return Semitones{lhs.noteNumber_ - rhs.noteNumber_};
}

inline Frequency MidiNoteNumber::hz() const {
  using namespace constants;
  return A4_FREQUENCY.transposedBy(*this - A4);
}

template <class IntervalType>
[[nodiscard]] MidiNoteNumber operator+(MidiNoteNumber lhs,
                                       const IntervalType& rhs) {
  lhs += rhs;
  return lhs;
}

template <class IntervalType>
[[nodiscard]] MidiNoteNumber operator+(const IntervalType& lhs,
                                       MidiNoteNumber rhs) {
  return rhs + lhs;
}

inline MidiNoteNumber Frequency::midi() const {
  using namespace constants;
  return A4 + octavesBetween(*this, A4_FREQUENCY);
}
}  // namespace wolfsound
