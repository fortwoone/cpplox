//
// Created by fortwoone on 05/04/2025.
//

#pragma once
#include <cstdint>
#include <ctime>
#include <memory>
#include "utils.hpp"

namespace lox::callable{
    using lox::ubyte;

    using std::time;
    using std::time_t;
    using std::shared_ptr;

    class AbstractLoxCallable;

    using CallablePtr = shared_ptr<AbstractLoxCallable>;

    using Value = variant<double, bool, string, CallablePtr>;
    using VarValue = Value;
    using EvalResult = Value;

    class AbstractLoxCallable{
        public:
            virtual ~AbstractLoxCallable() = default;
            [[nodiscard]] virtual constexpr ubyte arity() const = 0;
            [[nodiscard]] virtual Value call(const vector<Value>& args) const = 0;
    };

    bool is_number(const Value& val);

    bool is_boolean(const Value& val);

    bool is_string(const Value& val);

    bool is_callable(const Value& val);

    namespace builtins{
        class ClockFunc: public AbstractLoxCallable{
            public:
                [[nodiscard]] constexpr ubyte arity() const final{
                    return 0;
                }
                [[nodiscard]] Value call(const vector<Value>& args) const final;
        };
    }
}
