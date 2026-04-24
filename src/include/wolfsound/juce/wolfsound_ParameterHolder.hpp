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
/** @brief Type-erased container of Parameter class instances

    In audio plugins, we need to both access individual parameters instances
    as well as iterate over all parameters. Sometimes we need a concrete
    Parameter object of a specified type (e.g., juce::AudioParameterFloat),
    but sometimes we want to treat the parameters as a collection
    (e.g., for state serialization purposes, presets, etc.).

    One approach is to use virtual inheritance and pointers-to-base (like
    juce::AudioProcessorValueTreeState does it). This has the downside
    of losing the type information and the need to either treat all
    parameters as floats in the [0, 1] range or to use dynamic_cast (which
    is slow and error-prone).

    Type erasure is an alternative pattern allowing storing a collection of
    strongly-typed objects that don't need a common base class.

    The Visitor pattern allows us to access all stored parameters in
    a type-safe manner. However, individual parameter access is not
    supported: capture the parameters by reference returned by the Builder's
    class member functions to access the individually (e.g., to display
    juce::AudioParameterChoice choices or to use JUCE's ParameterAttachment
    classes).

    You can customize the supported parameter types by supplying a different
    Visitor base as the template class argument. The accept() member function
    requires that the supplied visitor is a child class of the Visitor template
    argument. For simplicity, I provide a JuceParameterVisitor that
    defines abstract methods for each JUCE Parameter class. You can inherit
    from it to add new methods. This default interface is used in the
    JuceParameterHolder.

    As an example, here's how you use the JuceParameterHolder (see associated
    unit tests as well).

    First, put is as a member of your PluginProcessor.

    @code
    private:
        wolfsound::JuceParameterHolder parameters;
    @endcode

    Then, list all parameters that you use with their types as references ABOVE
    the ParameterHolder declaration.

    @code
    private:
        juce::AudioParameterFloat& floatParam;
        juce::AudioParameterBool& boolParam;
        juce::AudioParameterInt& intParam;
        juce::AudioParameterChoice& choiceParam;
        wolfsound::JuceParameterHolder parameters;
    @endcode

    Pass an instance of JuceParameterHolder::Builder to your PluginProcessor's
    constructor (may be as an argument with a default value), and use its
    add() function to add parameters in the initializer list. Finally,
    call .build(*this) on the builder (not the you need to move it first).
    @code
    // header file
    explicit PluginProcessor(JuceParameterHolder::Builder builder = {});

    // implementation file
    PluginProcessor::PluginProcessor(
        JuceParameterHolder::Builder builder)
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
    @endcode

    Now you can access the parameters individually via references, or
    as a collection by "visiting" the ParameterHolder instance.
    See toVarArray() and update() for examples on how to do this.

    For the interested, the std::move(builder) requirement ensures that the
    builder is not used after calling build(). If it is, the code
    won't compile.

    To understand the Type Erasure pattern and the Visitor pattern,
    I recommend Klaus Iglberger's book "C++ Software Design."

    @tparam Visitor base class of all supported visitors. Should contain a void
    visit(ParameterClass&) abstract method for every supported Parameter
    class, e.g.,
    @code
    struct Visitor {
      virtual ~Visitor() = default;
      virtual void visit(juce::AudioParameterBool&) = 0;
      virtual void visit(juce::AudioParameterFloat&) = 0;
      //...
      virtual void visit(MyCustomParameter&) = 0;
    };
    @endcode

    @see JuceParameterVisitor, JuceParameterHolder
 */
template <class Visitor>
class ParameterHolder {
  /** @brief Internal type erasure wrapper for parameter classes. */
  class TypeErasedParameter {
  public:
    template <class Parameter>
    explicit TypeErasedParameter(Parameter& p)
        : _impl{std::make_unique<ParameterModel<Parameter>>(p)} {}

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
  /** @brief Use this class to define, instantiate, and collect Parameter
      instances, and to then construct a valid ParameterHolder containing them.
   */
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

    /** @brief Create the ParameterHolder while destroying the Builder

        You must call this method as std::move(builder).build(...).

        One-time operations that destroy the object should be qualified with &&
        to make it explicit at the call site
        (see
       https://www.foonathan.net/2018/03/rvalue-references-api-guidelines/)
     */
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

  /** @brief Construct an empty ParameterHolder.

      Use this only if you don't have any parameters.
  */
  ParameterHolder() = default;

  /** @brief Iterate over all parameters calling the appropriate Visitor method
      on each one. */
  void accept(Visitor& v) {
    for (auto& parameter : _parameters) {
      parameter.accept(v);
    }
  }

private:
  explicit ParameterHolder(std::vector<TypeErasedParameter> parameters)
      : _parameters{std::move(parameters)} {}

  std::vector<TypeErasedParameter> _parameters;
};

/** @brief Example ParameterHolder visitor that collects parameters' IDs and
    values in an array of juce::DynamicObject objects */
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

/** @brief Shorthand way of using VarArrayVisitor */
template <class Visitor>
juce::Array<juce::var> toVarArray(ParameterHolder<Visitor>& ph) {
  VarArrayVisitor<Visitor> visitor;
  ph.accept(visitor);
  return visitor.result();
}

/** @brief Updates parameters in the holder based on values in the passed-in
   array

    The array should contain juce::DynamicObject instances, each having
    an "id" field with a string and a "value" field with the parameter value,
    exactly like the array in SerializedParameters.

    @see SerializedParameters
*/
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

/** @brief Shorthand way of using UpdatingVisitor */
template <class Visitor>
void update(ParameterHolder<Visitor>& ph,
            const juce::Array<juce::var>& parameters) {
  UpdatingVisitor<Visitor> visitor{parameters};
  ph.accept(visitor);
}

/** @brief Example visitor supporting all Parameter classes shipped with JUCE.

    You can use is as the base class of your Visitor bases (if you want to
    support custom parameter types.
*/
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

/** @brief Use this class as a default ParameterHolder if you don't use custom
    Parameter classes */
using JuceParameterHolder = ParameterHolder<JuceParameterVisitor>;
}  // namespace wolfsound