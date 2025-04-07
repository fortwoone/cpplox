//
// Created by fortwoone on 02/04/2025.
//

#include "env.hpp"

#include <utility>

namespace lox::env{
    // The top-level environment does not have a parent environment,
    // so its enclosing member is set to nullptr.
    Environment::Environment(): enclosing(nullptr){}

    // Allows for nested variable scopes.
    Environment::Environment(const shared_ptr<Environment>& enclosing_env): enclosing(enclosing_env){}

    VarValue Environment::get(const string& name){  // NOLINT
        try{
            return vars.at(name);
        }
        catch (const out_of_range& exc){
            if (enclosing != nullptr){
                return enclosing->get(name);
            }
            throw runtime_error("Attempting to access nonexistent variable '" + name + "'");
        }
    }

    void Environment::set(const string& name, VarValue value){
        vars.insert_or_assign(name, value);
    }

    void Environment::assign(const string& name, VarValue value){  // NOLINT
        if (!vars.contains(name)){
            if (enclosing != nullptr){
                return enclosing->assign(name, value);
            }
            throw runtime_error("Undefined variable '" + name + "'");
        }
        vars[name] = std::move(value);
    }

    namespace for_callable{
        shared_ptr<Environment> get_func_env(const shared_ptr<Environment>& globals_env){
            return make_shared<Environment>(globals_env);
        }

        void set_env_member(const shared_ptr<Environment>& func_env, const string& name, VarValue value){
            func_env->set(name, value);
        }
    }
}