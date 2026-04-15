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
#include <memory>
#include <vector>
#include <functional>
// use juce_audio_processors instead of juce_audio_processors_headless
// for backward compatibility
#include <juce_audio_processors/juce_audio_processors.h>

namespace wolfsound {
template <class Visitor>
class ParameterHolder {
  class TypeErasedParameter {
  public:
    template <class Parameter>
    explicit TypeErasedParameter(Parameter& p)
        : _impl{std::make_unique<ParameterModel<Parameter>>(p)} {}

    /**
     * The Visitor class is expected to have a visit() member function
     * for each supported parameter type, e.g.,
     *
     *   struct Visitor {
     *     void visit(juce::AudioParameterBool&);
     *     void visit(juce::AudioParameterFloat&);
     *     //...
     *   };
     */
    void accept(Visitor& v) { _impl->accept(v); }

  private:
    class ParameterConcept {  // NOLINT
    public:
      virtual ~ParameterConcept() = default;
      virtual void accept(Visitor& v) = 0;
    };

    template <class Parameter>
    class ParameterModel : public ParameterConcept {
    public:
      explicit ParameterModel(Parameter& p) : _p{p} {}

      void accept(Visitor& v) override { v.visit(_p.get()); }

    private:
      std::reference_wrapper<Parameter> _p;
    };

    std::unique_ptr<ParameterConcept> _impl;
  };

public:
  class Builder {
  public:
    template <class P, class... Args>
    P& add(Args&&... args) {
      auto parameter = std::make_unique<P>(std::forward<Args>(args)...);
      auto& ref = *parameter;
      _parametersForHolder.emplace_back(ref);
      _parameters.push_back(std::move(parameter));
      return ref;
    }

    // one-time operations that destroy the object should be qualified with &&
    // to make it explicit at the call site
    // (see https://www.foonathan.net/2018/03/rvalue-references-api-guidelines/)
    ParameterHolder<Visitor> build(juce::AudioProcessor& p) && {
      for (auto&& parameter : _parameters) {
        p.addParameter(parameter.release());
      }
      return ParameterHolder{std::move(_parametersForHolder)};
    }

  private:
    std::vector<std::unique_ptr<juce::AudioProcessorParameter>> _parameters;
    std::vector<TypeErasedParameter> _parametersForHolder;
  };

  explicit ParameterHolder(std::vector<TypeErasedParameter> parameters)
      : _parameters{std::move(parameters)} {}

  void accept(Visitor& v) {
    for (auto& parameter : _parameters) {
      parameter.accept(v);
    }
  }

private:
  std::vector<TypeErasedParameter> _parameters;
};

template <class Visitor>
class VarArrayVisitor : public Visitor {
public:
  void visit(juce::AudioParameterFloat& parameter) override {
    visitImpl(parameter, parameter.get());
  }

  void visit(juce::AudioParameterBool& parameter) override {
    visitImpl(parameter, parameter.get());
  }

  void visit(juce::AudioParameterInt& parameter) override {
    visitImpl(parameter, parameter.get());
  }

  void visit(juce::AudioParameterChoice& parameter) override {
    visitImpl(parameter, parameter.getCurrentChoiceName());
  }

  [[nodiscard]] juce::Array<juce::var> result() const { return _result; }

private:
  template <class P, class V>
  void visitImpl(P& parameter, V value) {
    const juce::DynamicObject::Ptr object{new juce::DynamicObject};
    object->setProperty("id", parameter.getParameterID());
    object->setProperty("value", std::move(value));
    _result.add(juce::var{object});
  }

  juce::Array<juce::var> _result;
};

template <class Visitor>
juce::Array<juce::var> toVarArray(ParameterHolder<Visitor>& ph) {
  VarArrayVisitor<Visitor> visitor;
  ph.accept(visitor);
  return visitor.result();
}

template <class Visitor>
class UpdatingVisitor : public Visitor {
public:
  explicit UpdatingVisitor(const juce::Array<juce::var>& parameters)
      : _parameters{parameters} {}

  void visit(juce::AudioParameterFloat& parameter) override {
    visitImpl(parameter, [&](const auto& v) {
      if (v.isDouble()) {
        parameter = static_cast<float>(v);
      }
    });
  }

  void visit(juce::AudioParameterBool& parameter) override {
    visitImpl(parameter, [&](const auto& v) {
      if (v.isBool()) {
        parameter = static_cast<bool>(v);
      }
    });
  }

  void visit(juce::AudioParameterInt& parameter) override {
    visitImpl(parameter, [&](const auto& v) {
      if (v.isInt()) {
        parameter = static_cast<int>(v);
      }
    });
  }

  void visit(juce::AudioParameterChoice& parameter) override {
    visitImpl(parameter, [&](const auto& v) {
      if (v.isString()) {
        const auto choiceName = v.toString();
        const auto choiceIndex = parameter.choices.indexOf(choiceName);
        // don't "clamp" incorrect values, ignore them
        if (0 <= choiceIndex && choiceIndex < parameter.choices.size()) {
          parameter = choiceIndex;
        }
      }
    });
  }

private:
  template <class P>
  void visitImpl(P& parameter,
                 std::function<void(const juce::var&)> assignment) {
    const auto it = std::ranges::find_if(_parameters, [&](const juce::var& p) {
      return p.hasProperty("id") && p["id"] == parameter.getParameterID();
    });
    if (it != _parameters.end() && it->hasProperty("value")) {
      const auto& value = (*it)["value"];
      assignment(value);
    }
  }

  const juce::Array<juce::var>& _parameters;
};

template <class Visitor>
void update(ParameterHolder<Visitor>& ph,
            const juce::Array<juce::var>& parameters) {
  UpdatingVisitor<Visitor> visitor{parameters};
  ph.accept(visitor);
}

struct JuceParameterVisitor {
  JuceParameterVisitor() = default;
  virtual ~JuceParameterVisitor() = default;
  JuceParameterVisitor(const JuceParameterVisitor&) = delete;
  JuceParameterVisitor& operator=(const JuceParameterVisitor&) = delete;
  JuceParameterVisitor(JuceParameterVisitor&&) = delete;
  JuceParameterVisitor& operator=(JuceParameterVisitor&&) = delete;

  virtual void visit(juce::AudioParameterBool&) = 0;
  virtual void visit(juce::AudioParameterFloat&) = 0;
  virtual void visit(juce::AudioParameterInt&) = 0;
  virtual void visit(juce::AudioParameterChoice&) = 0;
};

using JuceParameterHolder = ParameterHolder<JuceParameterVisitor>;
}  // namespace wolfsound