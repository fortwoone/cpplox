//
// Created by fortwoone on 02/04/2025.
//

#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>

namespace lox::env{
    using std::shared_ptr;
    using std::out_of_range;
    using std::runtime_error;
    using std::string;
    using std::unordered_map;
    using std::variant;

    using VarValue = variant<double, bool, string>;

    class Environment{
        unordered_map<string, VarValue> vars;
        shared_ptr<Environment> enclosing; // Reference to parent environment for local scopes.

        public:
            Environment();

            explicit Environment(const shared_ptr<Environment>& enclosing_env);

            [[nodiscard]] VarValue get(const string& name);

            void set(const string& name, VarValue val);

            void assign(const string& name, VarValue val);
    };
}
