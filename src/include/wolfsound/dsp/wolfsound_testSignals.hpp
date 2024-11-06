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
#include <numbers>
#include <vector>
#include <ranges>
#include <wolfsound/common/wolfsound_Frequency.hpp>
#include <wolfsound/common/wolfsound_assert.hpp>
#include <wolfsound/common/wolfsound_mathFunctions.hpp>

namespace wolfsound {
using Seconds = std::chrono::duration<float>;

inline std::size_t samplesCountFrom(Frequency sampleRate, Seconds duration) {
  return static_cast<std::size_t>(
      std::round(static_cast<float>(duration.count()) * sampleRate.value()));
}

inline std::vector<float> generateSine(Frequency frequency,
                                       Frequency sampleRate,
                                       Seconds duration) {
  WS_PRECONDITION(frequency >= 0_Hz);
  WS_PRECONDITION(sampleRate > 0_Hz);

  const auto samplesCount = samplesCountFrom(sampleRate, duration);
  std::vector<float> result(samplesCount);

  constexpr auto TWO_PI = 2.f * std::numbers::pi_v<float>;
  const auto phaseIncrement = TWO_PI * frequency.value() / sampleRate.value();
  std::generate(result.begin(), result.end(),
                [phase = 0.f, phaseIncrement]() mutable {
                  const auto value = std::sin(phase);
                  phase = std::fmod(phase + phaseIncrement, TWO_PI);
                  return value;
                });

  return result;
}

inline std::vector<float> generateSquare(Frequency frequency,
                                         Frequency sampleRate,
                                         Seconds duration) {
  auto sine = generateSine(frequency, sampleRate, duration);

  std::ranges::for_each(sine,
                        [](auto& sample) { sample = wolfsound::sign(sample); });

  return sine;
}

/** @brief Harmonic-wise saw generation.
 *
 * @details Uses formula 2.1 from Marek Pluta
 * "Sound Synthesis for Music Reproduction and Performance", available online:
 * https://winntbg.bg.agh.edu.pl/skrypty4/0612/
 */
inline std::vector<float> generateNonaliasingSawRampDown(Frequency frequency,
                                                         Frequency sampleRate,
                                                         Seconds duration) {
  using namespace std::views;

  WS_PRECONDITION(frequency >= 0_Hz);
  WS_PRECONDITION(sampleRate > 0_Hz);
  WS_PRECONDITION(duration.count() > 0.f);

  const auto samplesCount = samplesCountFrom(sampleRate, duration);
  std::vector<float> result(samplesCount, 0.f);

  // determine the number of harmonics to generate
  const auto nyquistFrequency = sampleRate / 2.f;
  const auto harmonicsCount =
      static_cast<int>(nyquistFrequency.value() / frequency.value());

  // compute helper constants
  const auto omegaFundamental =
      2.f * std::numbers::pi_v<float> * frequency.value() / sampleRate.value();

  // generate each harmonic separately
  for (const auto harmonicIndex : iota(1) | take(harmonicsCount)) {
    const auto k = static_cast<float>(harmonicIndex);
    const auto sign = harmonicIndex % 2 == 0 ? 1.f : -1.f;
    const auto inverseK = 1.f / k;
    const auto omegaHarmonic = omegaFundamental * k;
    for (const auto sample : iota(0u, samplesCount)) {
      const auto floatSample = static_cast<float>(sample);
      result[sample] += sign * inverseK * std::sin(omegaHarmonic * floatSample);
    }
  }

  constexpr auto TWO_OVER_PI = 2.f / std::numbers::pi_v<float>;
  std::ranges::for_each(result, [](float& sample) { sample *= TWO_OVER_PI; });

  return result;
}
}  // namespace wolfsound
