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
#include <array>
#include <algorithm>
#include <cmath>
#include <wolfsound/common/wolfsound_assert.hpp>

namespace wolfsound {
template <typename SampleType>
class FractionalDelayLine {
public:
  [[nodiscard]] SampleType popSample() const;
  [[nodiscard]] SampleType popSample(SampleType delay) const;
  void pushSample(SampleType inputSample);
  void setDelay(SampleType newDelay);
  void reset();

private:
  static constexpr auto DELAY_LINE_LENGTH = 48000u;

  [[nodiscard]] SampleType clampDelay(SampleType delay) const noexcept;

  std::array<SampleType, DELAY_LINE_LENGTH> buffer_{};

  SampleType delay_ = 4.f;
  int writeHead_ = 0;
};

template <typename SampleType>
SampleType FractionalDelayLine<SampleType>::popSample() const {
  return popSample(delay_);
}

template <typename SampleType>
void FractionalDelayLine<SampleType>::pushSample(SampleType inputSample) {
  buffer_[static_cast<size_t>(writeHead_)] = inputSample;
  writeHead_++;
  writeHead_ %= static_cast<int>(std::ssize(buffer_));
}

template <typename SampleType>
void FractionalDelayLine<SampleType>::setDelay(SampleType newDelay) {
  WS_PRECONDITION(newDelay < static_cast<SampleType>(DELAY_LINE_LENGTH));
  delay_ = clampDelay(newDelay);
}

template <typename SampleType>
SampleType FractionalDelayLine<SampleType>::popSample(SampleType delay) const {
  constexpr auto DELAY_LINE_LENGTH_FLOAT =
      static_cast<SampleType>(DELAY_LINE_LENGTH);

  WS_PRECONDITION(delay >= 0.f);
  WS_PRECONDITION(delay < DELAY_LINE_LENGTH_FLOAT);

  auto readHead = static_cast<SampleType>(writeHead_) - 1 - delay;
  if (readHead < 0.f) {
    readHead += DELAY_LINE_LENGTH_FLOAT;
  }

  auto truncatedReadHead = std::floor(clampDelay(readHead));
  auto truncatedReadHeadPlusOne = truncatedReadHead + 1.f;

  const auto truncatedReadHeadWeight =
      std::abs(readHead - truncatedReadHeadPlusOne);
  const auto truncatedReadHeadPlusOneWeight =
      std::abs(readHead - truncatedReadHead);
  if (truncatedReadHeadPlusOne >= DELAY_LINE_LENGTH_FLOAT) {
    truncatedReadHeadPlusOne -= DELAY_LINE_LENGTH_FLOAT;
  }

  WS_ASSERT(0 <= truncatedReadHead, "invalid implementation");
  WS_ASSERT(0 <= truncatedReadHeadPlusOne, "invalid implementation");
  WS_ASSERT(truncatedReadHead < DELAY_LINE_LENGTH_FLOAT,
            "invalid implementation");
  WS_ASSERT(truncatedReadHeadPlusOne < DELAY_LINE_LENGTH_FLOAT,
            "invalid implementation");

  const auto outputSample =
      (truncatedReadHeadWeight *
       buffer_[static_cast<size_t>(truncatedReadHead)]) +
      (truncatedReadHeadPlusOneWeight *
       buffer_[static_cast<size_t>(truncatedReadHeadPlusOne)]);
  return outputSample;
}

template <typename SampleType>
void FractionalDelayLine<SampleType>::reset() {
  std::ranges::fill(buffer_, SampleType{0});
}

template <typename SampleType>
SampleType FractionalDelayLine<SampleType>::clampDelay(
    SampleType delay) const noexcept {
  return std::clamp(delay, SampleType{0},
                    static_cast<SampleType>(DELAY_LINE_LENGTH - 1u));
}
}  // namespace wolfsound
