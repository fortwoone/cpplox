//
// Created by fortwoone on 09/04/2025.
//

#pragma once
#include "callable.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace lox::inst{
    using lox::callable::LoxClass;
    using lox::callable::VarValue;

    using std::enable_shared_from_this;
    using std::make_shared;
    using std::runtime_error;
    using std::shared_ptr;
    using std::string;
    using std::unordered_map;

    using ClassPtr = shared_ptr<LoxClass>;

    class LoxInstance: public enable_shared_from_this<LoxInstance>{
        ClassPtr cls;
        unordered_map<string, VarValue> fields;

        public:
            explicit LoxInstance(const ClassPtr& klass);

            [[nodiscard]] string to_string() const{
                return cls->to_string() + " instance";
            }

            [[nodiscard]] VarValue get_attr(const string& name);

            void set_attr(const string& name, VarValue val);
    };

    namespace for_callable{
        shared_ptr<LoxInstance> create_inst(const ClassPtr& cls);
    }
}
