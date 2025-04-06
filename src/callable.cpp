//
// Created by fortwoone on 05/04/2025.
//

#include "callable.hpp"

namespace lox::callable{
    bool is_number(const Value& val){
        return holds_alternative<double>(val);
    }

    bool is_boolean(const Value& val){
        return holds_alternative<bool>(val);
    }

    bool is_string(const Value& val){
        return holds_alternative<string>(val);
    }

    bool is_callable(const Value& val){
        return holds_alternative<CallablePtr>(val);
    }

    namespace builtins{
        Value ClockFunc::call(const vector<Value> &args) const {
            return static_cast<double>(time(nullptr));
        }
    }
}