//
// Created by fortwoone on 02/04/2025.
//

#pragma once
#include "callable.hpp"
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <variant>

namespace lox::env{
    using std::make_shared;
    using std::out_of_range;
    using std::runtime_error;
    using std::shared_ptr;
    using std::string;
    using std::unordered_map;
    using std::variant;

    using lox::callable::VarValue;

    class Environment{
        unordered_map<string, VarValue> vars;
        shared_ptr<Environment> enclosing; // Reference to parent environment for local scopes.

        public:
            Environment();

            explicit Environment(const shared_ptr<Environment>& enclosing_env);

            [[nodiscard]] shared_ptr<Environment> get_enclosing(){
                return enclosing;
            }

            [[nodiscard]] VarValue get(const string& name);

            void set(const string& name, VarValue val);

            void assign(const string& name, VarValue val);
    };

    namespace for_callable{
        void set_env_member(const shared_ptr<Environment>& func_env, const string& name, VarValue value);
    }
}
