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
    using std::enable_shared_from_this;
    using std::make_shared;
    using std::out_of_range;
    using std::runtime_error;
    using std::shared_ptr;
    using std::string;
    using std::unordered_map;
    using std::variant;

    using lox::callable::VarValue;

    class Environment;

    using EnvPtr = shared_ptr<Environment>;

    class Environment: public enable_shared_from_this<Environment>{
        unordered_map<string, VarValue> vars;
        EnvPtr enclosing; // Reference to parent environment for local scopes.

        public:
            Environment();

            explicit Environment(const EnvPtr& enclosing_env);

            [[nodiscard]] EnvPtr get_enclosing(){
                return enclosing;
            }

            [[nodiscard]] VarValue get(const string& name);

            void set(const string& name, VarValue val);

            void assign(const string& name, VarValue val);

            [[nodiscard]] EnvPtr get_ancestor(size_t distance);

            [[nodiscard]] VarValue get_at(size_t distance, const string& name);

            void assign_at(size_t distance, const string& name, VarValue val);
    };

    namespace for_callable{
        EnvPtr get_child_env(const EnvPtr& orig);

        VarValue value_of_this(const EnvPtr& orig);

        void set_env_member(const EnvPtr& func_env, const string& name, VarValue value);
    }
}
