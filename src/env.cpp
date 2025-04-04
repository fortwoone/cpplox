//
// Created by fortwoone on 02/04/2025.
//

#include "env.hpp"

namespace lox::env{
    VarValue Environment::get(const string& name){
        try{
            return vars.at(name);
        }
        catch (const out_of_range& exc){
            throw runtime_error("Attempting to access nonexistent variable '" + name + "'");
        }
    }

    void Environment::set(const string& name, VarValue value){
        vars.insert_or_assign(name, value);
    }
}